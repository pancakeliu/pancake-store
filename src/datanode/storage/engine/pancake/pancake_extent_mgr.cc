
#include "src/datanode/storage/engine/pancake/pancake_extent_mgr.h"

#include <seastar/core/coroutine.hh>
#include <seastar/core/scollectd.hh>

namespace pancake_store::datanode::storage {

using pancake_store::comm::ErrorCodeName;
using pancake_store::comm::IsNotOk;

ErrorCode PancakeExtentManager::Init(DeviceOffset offset, uint32_t bitmap_block_count, uint32_t extent_block_count,
                             uint32_t extent_block_size, DeviceOffset extent_blocks_offset) {
    auto max_blocks = bitmap_block_count * static_cast<uint32_t>(k_block_data_size * sizeof(uint8_t));
    if (extent_block_count > max_blocks) {
        return ErrorCode::DATANODE_STORAGE_FS_INIT_FAILED;
    }

    if (!PancakeComm::IsBlockAlign(offset)) {
        logger_.error("extent bitmap offset:{} not block aligned", offset);
        return ErrorCode::DATANODE_STORAGE_FS_INIT_FAILED;
    }
    if (!PancakeComm::IsBlockAlign(extent_blocks_offset)) {
        logger_.error("extent blocks offset:{} not block aligned", extent_blocks_offset);
        return ErrorCode::DATANODE_STORAGE_FS_INIT_FAILED;
    }

    offset_ = offset;
    bitmap_block_count_ = bitmap_block_count;
    extent_block_count_ = extent_block_count;
    extent_block_size_ = extent_block_size;
    extent_blocks_offset_ = extent_blocks_offset;

    // init extent bitmap
    bitmap_.resize(extent_block_count_, false);

    // clear extent offset map
    extent_offset_map_.clear();

    return ErrorCode::PANCAKE_STORE_OK;
}

seastar::future<ErrorCode> PancakeExtentManager::AddExtent(ExtentHeaderBlock *ext_header) {
    uint32_t ext_block_index = std::numeric_limits<uint32_t>::max();

    return seastar::with_lock(mutex_, [this, &ext_header, &ext_block_index] {
        if (extent_offset_map_.contains(ext_header->extent_id_)) {
            logger_.error("extent:{} already added in this device.", ext_header->extent_id_);
            return seastar::make_ready_future<ErrorCode>(ErrorCode::DATANODE_STORAGE_EXTENT_ALREADY_EXISTS);
        }

        // extent block alloc full
        if (used_cnt_ == extent_block_count_) {
            logger_.error("extent block full!!!");
            return seastar::make_ready_future<ErrorCode>(ErrorCode::DATANODE_STORAGE_FS_FULL);
        }
        // find unused extent block
        for (uint32_t i = 0; i < extent_block_count_; i++) {
            cursor_ = cursor_ % bitmap_block_count_;
            if (!bitmap_[cursor_]) {
                ext_block_index = cursor_;
                cursor_++;
                break;
            }
            cursor_++;
        }
        if (ext_block_index == std::numeric_limits<uint32_t>::max()) {
            logger_.error("code error!!!! not found usable extent block.");
            return seastar::make_ready_future<ErrorCode>(ErrorCode::PANCAKE_STORE_CODE_ERROR);
        }

        // got extent block
        DeviceOffset ext_device_offset = extentBlockDeviceOffset(ext_block_index);

        logger_.info("start to alloc extent:{} into extent block:{}, block_offset:{}",
            ext_header->extent_id_, ext_block_index, ext_device_offset);

        // set ext block index into ext header
        ext_header->bitmap_ext_index_ = ext_block_index;

        auto ext_header_bytes = ext_header->Serialize();
        ErrorCode err_code = co_await pancake_io_->WriteAt(ext_device_offset, ext_header_bytes);
        if (IsNotOk(err_code)) {
            logger_.error("extent block header write at failed. err:{}", ErrorCodeName(err_code));
            return seastar::make_ready_future<ErrorCode>(err_code);
        }

        // update extent bitmap block
        auto bitmap_block = cloneBitMapBlockWithoutLock(ext_block_index);
        // mark ext_block_index used
        markExtentBlockUsed(&bitmap_block, ext_block_index);

        // set footer
        bitmap_block.footer_.FillWithoutExtentId(sizeof(bitmap_block.bitmap_));

        const auto bitmap_block_offset = bitmapBlockDeviceOffset(bitmapIndex(ext_block_index));
        const auto bitmap_block_bytes = bitmap_block.Serialize();
        err_code = co_await pancake_io_->WriteAt(bitmap_block_offset, bitmap_block_bytes);
        if (IsNotOk(err_code)) {
            logger_.error("bitmap block write at failed. err:{}", ErrorCodeName(err_code));
            return seastar::make_ready_future<ErrorCode>(err_code);
        }

        // update block bitmap
        bitmap_[ext_block_index] = true;
        extent_offset_map_.insert(ext_header->extent_id_, ext_device_offset);

        logger_.info("add extent success. extent:{}, bitmap_index:{}",
                     ext_header->extent_id_, ext_block_index);

        return seastar::make_ready_future<ErrorCode>(ErrorCode::PANCAKE_STORE_OK);
    });
}

seastar::future<ErrorCode> PancakeExtentManager::DelExtent(const ExtentHeaderBlock &ext_header) {
    logger_.info("start to delete extent:{}, bitmap_index:{}",
                 ext_header.extent_id_, ext_header.bitmap_ext_index_);

    return seastar::with_lock(mutex_, [this, &ext_header] {
        if (!extent_offset_map_.contains(ext_header.extent_id_)) {
            logger_.error("extent:{} not found in this device.", ext_header.extent_id_);

        }

        // double check before delete
        const auto extent_block_offset = extentBlockDeviceOffset(ext_header.bitmap_ext_index_);

        ExtentHeaderBlock tmp_ext_header;
        seastar::sstring read_buf{};
        auto err_code = co_await pancake_io_->ReadAt(extent_block_offset, sizeof(tmp_ext_header), &read_buf);
        if (IsNotOk(err_code)) {
            logger_.error("read at block failed. err:{}, extent:{}, offset:{}",
                ErrorCodeName(err_code), ext_header.extent_id_, extent_block_offset);
            return seastar::make_ready_future<ErrorCode>(err_code);
        }

        // if crc error, do not need check extent id

        err_code = tmp_ext_header.Deserialize(read_buf);
        if (IsNotOk(err_code)) {
            logger_.error("extent header block deserialize failed. err:{}, extent:{}, offset:{}",
                ErrorCodeName(err_code), ext_header.extent_id_, extent_block_offset);
            return seastar::make_ready_future<ErrorCode>(err_code);
        }

        // check extent equal
        // TODO(pancake): maybe device error??
        if (tmp_ext_header.extent_id_ != ext_header.extent_id_) {
            logger_.error("extent id not equal, can not delete. extent:{}, device_ext:{}",
                ext_header.extent_id_, tmp_ext_header.extent_id_);
            return seastar::make_ready_future<ErrorCode>(ErrorCode::DATANODE_STORAGE_INTERNAL_ERROR);
        }

        // check success, update extent block bitmap
        auto bitmap_block = cloneBitMapBlockWithoutLock(ext_header.bitmap_ext_index_);
        // mark ext_block_index used
        markExtentBlockUnused(&bitmap_block, ext_header.bitmap_ext_index_);

        // set footer
        bitmap_block.footer_.FillWithoutExtentId(sizeof(bitmap_block.bitmap_));

        const auto bitmap_block_offset = bitmapBlockDeviceOffset(bitmapIndex(ext_header.bitmap_ext_index_));
        const auto bitmap_block_bytes = bitmap_block.Serialize();
        err_code = co_await pancake_io_->WriteAt(bitmap_block_offset, bitmap_block_bytes);
        if (IsNotOk(err_code)) {
            logger_.error("bitmap block write at failed. err:{}", ErrorCodeName(err_code));
            return seastar::make_ready_future<ErrorCode>(err_code);
        }

        // update block bitmap
        bitmap_[ext_header.bitmap_ext_index_] = false;

        return seastar::make_ready_future<ErrorCode>(ErrorCode::PANCAKE_STORE_OK);
    });
}

//seastar::future<std::pair<ErrorCode, DeviceOffset>> PancakeExtentManager::ExtentDeviceOffset(ExtentId extent_id) {
//    return seastar::with_lock(mutex_, [this] {
//        auto iter =
//    });
//}

seastar::future<uint32_t> PancakeExtentManager::CurrentAllocCount() {
    return seastar::with_lock(mutex_, [this] {
        return seastar::make_ready_future<uint32_t>(used_cnt_);
    });
}

seastar::future<uint32_t> PancakeExtentManager::CurrentFreeCount() {
    return seastar::with_lock(mutex_, [this] {
        return seastar::make_ready_future<uint32_t>(extent_block_count_ - used_cnt_);
    });
}

BitMapBlock PancakeExtentManager::cloneBitMapBlockWithoutLock(uint32_t ext_block_index) {
    BitMapBlock bitmap_block{};
    memset(&bitmap_block, 0, sizeof(bitmap_block));

    const int block_index = bitmapIndex(ext_block_index);
    const int start = block_index * static_cast<int>(k_block_data_size * sizeof(uint8_t));
    int end = start + static_cast<int>(k_block_data_size * sizeof(uint8_t));
    end = std::min(end, static_cast<int>(extent_block_count_));

    for (int i = start; i < end; i++) {
        const int inner_index = (i - start) / static_cast<int>(sizeof(uint8_t));
        const int inner_offset = (i - start) % static_cast<int>(sizeof(uint8_t));

        // false
        if (!bitmap_[i]) {
            bitmap_block.bitmap_[inner_index] &= ~(1 << inner_offset);
            continue;
        }
        // ture
        bitmap_block.bitmap_[inner_index] |= (1 << inner_offset);
    }

    return bitmap_block;
}

void PancakeExtentManager::markExtentBlockUsed(BitMapBlock *bitmap_block, uint32_t ext_block_index) {
    const int inner_ext_index = ext_block_index % ((k_block_data_size * sizeof(uint8_t)));
    const int inner_index = inner_ext_index / static_cast<int>(sizeof(uint8_t));
    const int inner_offset = inner_ext_index % static_cast<int>(sizeof(uint8_t));

    // mark used
    bitmap_block->bitmap_[inner_index] |= (1 << inner_offset);
}

void PancakeExtentManager::markExtentBlockUnused(BitMapBlock *bitmap_block, uint32_t ext_block_index) {
    const int inner_ext_index = ext_block_index % ((k_block_data_size * sizeof(uint8_t)));
    const int inner_index = inner_ext_index / static_cast<int>(sizeof(uint8_t));
    const int inner_offset = inner_ext_index % static_cast<int>(sizeof(uint8_t));

    bitmap_block->bitmap_[inner_index] &= ~(1 << inner_offset);
}

inline uint32_t PancakeExtentManager::bitmapIndex(const uint64_t extent_block_index) {
    return extent_block_index / (k_block_data_size * sizeof(uint8_t));
}

inline DeviceOffset PancakeExtentManager::bitmapBlockDeviceOffset(const uint32_t bitmap_block_index)const {
    return offset_ + (bitmap_block_index * k_block_size);
}

inline DeviceOffset PancakeExtentManager::extentBlockDeviceOffset(uint32_t extent_block_index)const {
    return (static_cast<uint64_t>(extent_block_index) * static_cast<uint64_t>(extent_block_size_)) +
           extent_blocks_offset_;
}

} // namespace pancake_store::datanode::storage