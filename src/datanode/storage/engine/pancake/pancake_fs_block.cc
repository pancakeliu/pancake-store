
#include "src/datanode/storage/engine/pancake/pancake_fs_block.h"
#include "src/comm/crc.h"

namespace pancake_store::datanode::storage {

using pancake_store::comm::CRC;

constexpr uint32_t SUPER_BLOCK_MAGIC = 0x20250430;

seastar::sstring SuperBlock::Serialize() {
    std::string result;
    result.reserve(k_block_size);

    auto append_data = [&result](const void* data, const size_t size) {
        result.append(static_cast<const char*>(data), size);
    };

    const uint32_t be_magic = seastar::net::hton(magic_);
    append_data(&be_magic, sizeof(be_magic));
    append_data(&version_, sizeof(version_));
    const uint64_t be_device_id = seastar::net::hton(device_id_);
    append_data(&be_device_id, sizeof(be_device_id));

    const uint16_t uuid_len = seastar::net::hton(static_cast<uint16_t>(device_uuid_.size()));
    append_data(&uuid_len, sizeof(uuid_len));
    append_data(device_uuid_.c_str(), device_uuid_.size());

    const uint16_t path_len = seastar::net::hton(static_cast<uint16_t>(device_path_.size()));
    append_data(&path_len, sizeof(path_len));
    append_data(device_path_.c_str(), device_path_.size());

    const int64_t be_create_time = seastar::net::hton(create_time_);
    append_data(&be_create_time, sizeof(be_create_time));

    const uint64_t be_device_capacity = seastar::net::hton(device_capacity_);
    append_data(&be_device_capacity, sizeof(be_device_capacity));

    const uint64_t be_block_size = seastar::net::hton(block_size_);
    append_data(&be_block_size, sizeof(be_block_size));

    const uint32_t be_journal_block_count = seastar::net::hton(journal_block_count_);
    append_data(&be_journal_block_count, sizeof(be_journal_block_count));

    const uint64_t be_extent_block_size = seastar::net::hton(extent_block_size_);
    append_data(&be_extent_block_size, sizeof(be_extent_block_size));

    const uint64_t be_extent_block_count = seastar::net::hton(extent_block_count_);
    append_data(&be_extent_block_count, sizeof(be_extent_block_count));

    const uint64_t be_extent_block_bitmap_start = seastar::net::hton(extent_block_bitmap_start_);
    append_data(&be_extent_block_bitmap_start, sizeof(be_extent_block_bitmap_start));

    const uint64_t be_extent_block_bitmap_end = seastar::net::hton(extent_block_bitmap_end_);
    append_data(&be_extent_block_bitmap_end, sizeof(be_extent_block_bitmap_end));

    const uint64_t be_trash_extent_block_bitmap_start = seastar::net::hton(trash_extent_block_bitmap_start_);
    append_data(&be_trash_extent_block_bitmap_start, sizeof(be_trash_extent_block_bitmap_start));

    const uint64_t be_trash_extent_block_bitmap_end = seastar::net::hton(trash_extent_block_bitmap_end_);
    append_data(&be_trash_extent_block_bitmap_end, sizeof(be_trash_extent_block_bitmap_end));

    const uint64_t be_journal_blocks_start = seastar::net::hton(journal_blocks_start_);
    append_data(&be_journal_blocks_start, sizeof(be_journal_blocks_start));

    const uint64_t be_journal_blocks_end = seastar::net::hton(journal_blocks_end_);
    append_data(&be_journal_blocks_end, sizeof(be_journal_blocks_end));

    const uint64_t be_extent_blocks_start = seastar::net::hton(extent_blocks_start_);
    append_data(&be_extent_blocks_start, sizeof(be_extent_blocks_start));

    crc_ = CRC::CRC32(result.data(), result.size());
    const uint32_t be_crc = seastar::net::hton(crc_);
    append_data(&be_crc, sizeof(be_crc));

    if (result.size() < k_block_size) {
        result.append(k_block_size - result.size(), '\0');
    }

    return result;
}

ErrorCode SuperBlock::Deserialize(const seastar::sstring& data) {
    if (data.size() != k_block_size) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_SIZE_ILLEGAL;
    }

    const char* ptr = data.data();
    size_t remaining = data.size();

    auto read_data = [&ptr, &remaining](void* dest, const size_t size) -> bool {
        if (remaining < size) {
            return false;
        }
        memcpy(dest, ptr, size);
        ptr += size;
        remaining -= size;
        return true;
    };

    uint32_t be_magic;
    if (!read_data(&be_magic, sizeof(be_magic))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }
    magic_ = seastar::net::ntoh(be_magic);

    if (magic_ != SUPER_BLOCK_MAGIC) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_MAGIC_ILLEGAL;
    }

    if (!read_data(&version_, sizeof(version_))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }

    uint64_t be_device_id;
    if (!read_data(&be_device_id, sizeof(be_device_id))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }
    device_id_ = seastar::net::ntoh(be_device_id);

    uint16_t uuid_len;
    if (!read_data(&uuid_len, sizeof(uuid_len))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }
    uuid_len = seastar::net::ntoh(uuid_len);

    if (uuid_len > 0) {
        if (remaining < uuid_len) {
            return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
        }
        device_uuid_ = seastar::sstring{ptr, uuid_len};
        ptr += uuid_len;
        remaining -= uuid_len;
    }

    uint16_t path_len;
    if (!read_data(&path_len, sizeof(path_len))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }
    path_len = seastar::net::ntoh(path_len);

    if (path_len > 0) {
        if (remaining < path_len) {
            return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
        }
        device_path_ = seastar::sstring{ptr, path_len};
        ptr += path_len;
        remaining -= path_len;
    }

    int64_t be_create_time;
    if (!read_data(&be_create_time, sizeof(be_create_time))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }
    create_time_ = seastar::net::ntoh(be_create_time);

    uint64_t be_device_capacity;
    if (!read_data(&be_device_capacity, sizeof(be_device_capacity))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }
    device_capacity_ = seastar::net::ntoh(be_device_capacity);

    uint64_t be_block_size;
    if (!read_data(&be_block_size, sizeof(be_block_size))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }
    block_size_ = seastar::net::ntoh(be_block_size);

    uint32_t be_journal_block_count;
    if (!read_data(&be_journal_block_count, sizeof(be_journal_block_count))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }
    journal_block_count_ = seastar::net::ntoh(be_journal_block_count);

    uint64_t be_extent_block_size;
    if (!read_data(&be_extent_block_size, sizeof(be_extent_block_size))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }
    extent_block_size_ = seastar::net::ntoh(be_extent_block_size);

    uint64_t be_extent_block_count;
    if (!read_data(&be_extent_block_count, sizeof(be_extent_block_count))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }
    extent_block_count_ = seastar::net::ntoh(be_extent_block_count);

    uint64_t be_extent_block_bitmap_start;
    if (!read_data(&be_extent_block_bitmap_start, sizeof(be_extent_block_bitmap_start))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }
    extent_block_bitmap_start_ = seastar::net::ntoh(be_extent_block_bitmap_start);

    uint64_t be_extent_block_bitmap_end;
    if (!read_data(&be_extent_block_bitmap_end, sizeof(be_extent_block_bitmap_end))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }
    extent_block_bitmap_end_ = seastar::net::ntoh(be_extent_block_bitmap_end);

    uint64_t be_trash_extent_block_bitmap_start;
    if (!read_data(&be_trash_extent_block_bitmap_start, sizeof(be_trash_extent_block_bitmap_start))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }
    trash_extent_block_bitmap_start_ = seastar::net::ntoh(be_trash_extent_block_bitmap_start);

    uint64_t be_trash_extent_block_bitmap_end;
    if (!read_data(&be_trash_extent_block_bitmap_end, sizeof(be_trash_extent_block_bitmap_end))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }
    trash_extent_block_bitmap_end_ = seastar::net::ntoh(be_trash_extent_block_bitmap_end);

    uint64_t be_journal_blocks_start;
    if (!read_data(&be_journal_blocks_start, sizeof(be_journal_blocks_start))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }
    journal_blocks_start_ = seastar::net::ntoh(be_journal_blocks_start);

    uint64_t be_journal_blocks_end;
    if (!read_data(&be_journal_blocks_end, sizeof(be_journal_blocks_end))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }
    journal_blocks_end_ = seastar::net::ntoh(be_journal_blocks_end);

    uint64_t be_extent_blocks_start;
    if (!read_data(&be_extent_blocks_start, sizeof(be_extent_blocks_start))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }
    extent_blocks_start_ = seastar::net::ntoh(be_extent_blocks_start);

    uint32_t be_crc;
    if (!read_data(&be_crc, sizeof(be_crc))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }
    crc_ = seastar::net::ntoh(be_crc);

    if (const uint32_t calculated_crc = CRC::CRC32(data.data(), ptr - data.data() - sizeof(crc_));calculated_crc != crc_) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_CRC_MISMATCH;
    }

    return ErrorCode::PANCAKE_STORE_OK;
}

// block footer


} //namespace pancake_store::datanode::storage