
#include "src/datanode/storage/engine/pancake/pancake_fs.h"

#include <seastar/core/coroutine.hh>

namespace pancake_store::datanode::storage {

using pancake_store::comm::ErrorCodeName;
using pancake_store::comm::IsNotOk;

ErrorCode ExtentBitMap::Init(uint64_t offset, uint32_t bitmap_block_count, uint32_t extent_block_count,
                             uint32_t extent_block_size, uint64_t extent_blocks_offset) {
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

    return ErrorCode::PANCAKE_STORE_OK;
}

seastar::future<ErrorCode> ExtentBitMap::AddExtent(const ExtentHeaderBlock &ext_header) {
    uint32_t ext_block_index = std::numeric_limits<uint32_t>::max();

    return seastar::with_lock(mutex_, [this, &ext_header, &ext_block_index] {
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
        uint64_t ext_device_offset = extent_blocks_offset_ +
            (static_cast<uint64_t>(ext_block_index) * static_cast<uint64_t>(extent_block_size_));

        logger_.info("start to alloc extent:{} into extent block:{}, block_offset:{}",
            ext_header.extent_id_, ext_block_index, ext_device_offset);

        auto ext_header_bytes = ext_header.Serialize();
        ErrorCode err_code = co_await pancake_io_->WriteAt(ext_device_offset, ext_header_bytes);
        if (IsNotOk(err_code)) {
            logger_.error("extent block header write at failed. err:{}", ErrorCodeName(err_code));
            return seastar::make_ready_future<ErrorCode>(err_code);
        }

        // update extent bitmap block

        return seastar::make_ready_future<ErrorCode>(ErrorCode::PANCAKE_STORE_OK);
    });
}

seastar::future<ErrorCode> ExtentBitMap::DelExtent(const ExtentHeaderBlock &ext_header) {

}

seastar::future<uint32_t> ExtentBitMap::CurrentAllocCount() {
    return seastar::with_lock(mutex_, [this] {
        return seastar::make_ready_future<uint32_t>(used_cnt_);
    });
}

seastar::future<uint32_t> ExtentBitMap::CurrentFreeCount() {
    return seastar::with_lock(mutex_, [this] {
        return seastar::make_ready_future<uint32_t>(extent_block_count_ - used_cnt_);
    });
}

} // namespace pancake_store::datanode::storage