
#include "src/datanode/storage/engine/pancake/pancake_fs_block.h"
#include "src/comm/crc.h"

namespace pancake_store::datanode::storage {

using pancake_store::comm::CRC;

seastar::sstring SuperBlock::Serialize() {
    std::string result;
    result.reserve(k_block_size);

    auto append_data = [&result](const void* data, const size_t size) {
        result.append(static_cast<const char*>(data), size);
    };

    magic_ = k_block_magic;
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

    assert(result.size() == k_block_size);

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

    if (magic_ != k_block_magic) {
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

seastar::sstring DataBlock::Serialize() {
    std::string result;
    result.reserve(k_block_size);

    auto append_data = [&result](const void* data, const size_t size) {
        result.append(static_cast<const char*>(data), size);
    };

    // append data
    append_data(&data_, sizeof(data_));

    // append footer
    footer_.magic_ = k_block_magic;
    const uint32_t be_magic = seastar::net::hton(footer_.magic_);
    append_data(&be_magic, sizeof(be_magic));

    append_data(&footer_.version_, sizeof(footer_.version_));
    append_data(&footer_.flag_, sizeof(footer_.flag_));

    const uint16_t be_length = seastar::net::hton(footer_.length_);
    append_data(&be_length, sizeof(be_length));

    const uint64_t be_extent_id = seastar::net::hton(footer_.extent_id_);
    append_data(&be_extent_id, sizeof(be_extent_id));

    append_data(&footer_.reserved_, sizeof(footer_.reserved_));

    footer_.crc_ = CRC::CRC32(result.data(), result.size());
    const uint32_t be_crc = seastar::net::hton(footer_.crc_);
    append_data(&be_crc, sizeof(be_crc));

    assert(result.size() == k_block_size);

    return result;
}

ErrorCode DataBlock::Deserialize(const seastar::sstring& data) {
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

    if (!read_data(data_, sizeof(data_))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }

    // footer deserialize
    uint32_t be_magic;
    if (!read_data(&be_magic, sizeof(be_magic))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }
    footer_.magic_ = seastar::net::ntoh(be_magic);
    if (footer_.magic_ != k_block_magic) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_MAGIC_ILLEGAL;
    }

    if (!read_data(&footer_.version_, sizeof(footer_.version_))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }

    if (!read_data(&footer_.flag_, sizeof(footer_.flag_))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }

    uint16_t be_length;
    if (!read_data(&be_length, sizeof(be_length))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }
    footer_.length_ = seastar::net::ntoh(be_length);

    uint64_t be_extent_id;
    if (!read_data(&be_extent_id, sizeof(be_extent_id))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }
    footer_.extent_id_ = seastar::net::ntoh(be_extent_id);

    if (!read_data(&footer_.reserved_, sizeof(footer_.reserved_))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }

    uint32_t be_crc;
    if (!read_data(&be_crc, sizeof(be_crc))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }

    footer_.crc_ = seastar::net::ntoh(be_crc);

    if (const uint32_t calculated_crc = CRC::CRC32(data.data(), ptr - data.data() - sizeof(footer_.crc_)); calculated_crc != footer_.crc_) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_CRC_MISMATCH;
    }

    return ErrorCode::PANCAKE_STORE_OK;
}

seastar::sstring JournalBlock::Serialize() {
    std::string result;
    result.reserve(k_block_size);

    auto append_data = [&result](const void* data, const size_t size) {
        result.append(static_cast<const char*>(data), size);
    };

    // append data
    append_data(&old_data_, sizeof(old_data_));

    // append footer
    journal_footer_.magic_ = k_block_magic;
    const uint32_t be_magic = seastar::net::hton(journal_footer_.magic_);
    append_data(&be_magic, sizeof(be_magic));

    append_data(&journal_footer_.version_, sizeof(journal_footer_.version_));
    append_data(&journal_footer_.flag_, sizeof(journal_footer_.flag_));

    const uint16_t be_length = seastar::net::hton(journal_footer_.length_);
    append_data(&be_length, sizeof(be_length));

    const uint64_t be_extent_id = seastar::net::hton(journal_footer_.extent_id_);
    append_data(&be_extent_id, sizeof(be_extent_id));

    const uint64_t be_transaction_id = seastar::net::hton(journal_footer_.transaction_id_);
    append_data(&be_transaction_id, sizeof(be_transaction_id));

    append_data(&journal_footer_.commited_flag_, sizeof(journal_footer_.commited_flag_));
    append_data(&journal_footer_.reserved_, sizeof(journal_footer_.reserved_));

    append_data(&journal_footer_.reserved_, sizeof(journal_footer_.reserved_));

    journal_footer_.crc_ = CRC::CRC32(result.data(), result.size());
    const uint32_t be_crc = seastar::net::hton(journal_footer_.crc_);
    append_data(&be_crc, sizeof(be_crc));

    assert(result.size() == k_block_size);

    return result;
}

ErrorCode JournalBlock::Deserialize(const seastar::sstring& data) {
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

    if (!read_data(old_data_, sizeof(old_data_))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }

    // footer deserialize
    uint32_t be_magic;
    if (!read_data(&be_magic, sizeof(be_magic))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }
    journal_footer_.magic_ = seastar::net::ntoh(be_magic);
    if (journal_footer_.magic_ != k_block_magic) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_MAGIC_ILLEGAL;
    }

    if (!read_data(&journal_footer_.version_, sizeof(journal_footer_.version_))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }

    if (!read_data(&journal_footer_.flag_, sizeof(journal_footer_.flag_))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }

    uint16_t be_length;
    if (!read_data(&be_length, sizeof(be_length))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }
    journal_footer_.length_ = seastar::net::ntoh(be_length);

    uint64_t be_extent_id;
    if (!read_data(&be_extent_id, sizeof(be_extent_id))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }
    journal_footer_.extent_id_ = seastar::net::ntoh(be_extent_id);

    uint64_t be_transaction_id;
    if (!read_data(&be_transaction_id, sizeof(be_transaction_id))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }
    journal_footer_.transaction_id_ = seastar::net::ntoh(be_transaction_id);

    if (!read_data(&journal_footer_.commited_flag_, sizeof(journal_footer_.commited_flag_))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }

    if (!read_data(&journal_footer_.reserved_, sizeof(journal_footer_.reserved_))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }

    uint32_t be_crc;
    if (!read_data(&be_crc, sizeof(be_crc))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }

    journal_footer_.crc_ = seastar::net::ntoh(be_crc);

    if (const uint32_t calculated_crc = CRC::CRC32(data.data(), ptr - data.data() - sizeof(journal_footer_.crc_));
        calculated_crc != journal_footer_.crc_) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_CRC_MISMATCH;
    }

    return ErrorCode::PANCAKE_STORE_OK;
}

seastar::sstring ExtentHeaderBlock::Serialize() const {
    std::string result;
    result.reserve(k_block_size);

    auto append_data = [&result](const void* data, const size_t size) {
        result.append(static_cast<const char*>(data), size);
    };

    magic_ = k_block_magic;
    const uint32_t be_magic = seastar::net::hton(magic_);
    append_data(&be_magic, sizeof(be_magic));

    append_data(&version_, sizeof(version_));
    append_data(&shard_id_, sizeof(shard_id_));

    // rep_shard_cnt_
    append_data(&data_shard_cnt_, sizeof(data_shard_cnt_));
    append_data(&code_shard_cnt_, sizeof(code_shard_cnt_));

    const uint64_t be_extent_id = seastar::net::hton(extent_id_);
    append_data(&be_extent_id, sizeof(be_extent_id));

    const uint32_t be_extent_capacity = seastar::net::hton(extent_capacity_);
    append_data(&be_extent_capacity, sizeof(be_extent_capacity));

    const uint32_t be_extent_phy_used = seastar::net::hton(extent_phy_used_);
    append_data(&be_extent_phy_used, sizeof(be_extent_phy_used));

    const uint32_t be_extent_logic_used = seastar::net::hton(extent_logic_used_);
    append_data(&be_extent_logic_used, sizeof(be_extent_logic_used));

    append_data(&redundant_type_, sizeof(redundant_type_));
    append_data(&sealed_, sizeof(sealed_));
    append_data(&is_migrating_, sizeof(is_migrating_));
    append_data(&corrupted_, sizeof(corrupted_));

    append_data(&reserved_, sizeof(reserved_));

    // append footer
    block_footer_.magic_ = k_block_magic;
    const uint32_t be_footer_magic = seastar::net::hton(block_footer_.magic_);
    append_data(&be_footer_magic, sizeof(be_footer_magic));

    append_data(&block_footer_.version_, sizeof(block_footer_.version_));
    append_data(&block_footer_.flag_, sizeof(block_footer_.flag_));

    const uint16_t be_length = seastar::net::hton(block_footer_.length_);
    append_data(&be_length, sizeof(be_length));

    const uint64_t be_footer_extent_id = seastar::net::hton(block_footer_.extent_id_);
    append_data(&be_footer_extent_id, sizeof(be_footer_extent_id));

    append_data(&block_footer_.reserved_, sizeof(block_footer_.reserved_));

    block_footer_.crc_ = CRC::CRC32(result.data(), result.size());
    const uint32_t be_crc = seastar::net::hton(block_footer_.crc_);
    append_data(&be_crc, sizeof(be_crc));

    assert(result.size() == k_block_size);

    return result;
}

ErrorCode ExtentHeaderBlock::Deserialize(const seastar::sstring& data) {
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
    if (magic_ != k_block_magic) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_MAGIC_ILLEGAL;
    }

    if (!read_data(&version_, sizeof(version_))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }

    if (!read_data(&shard_id_, sizeof(shard_id_))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }

    // rep_shard_cnt
    if (!read_data(&data_shard_cnt_, sizeof(data_shard_cnt_))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }

    uint64_t be_extent_id;
    if (!read_data(&be_extent_id, sizeof(be_extent_id))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }
    extent_id_ = seastar::net::ntoh(be_extent_id);

    uint32_t be_extent_capacity;
    if (!read_data(&be_extent_capacity, sizeof(be_extent_capacity))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }
    extent_capacity_ = seastar::net::ntoh(be_extent_capacity);

    uint32_t be_extent_phy_used;
    if (!read_data(&be_extent_phy_used, sizeof(be_extent_phy_used))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }
    extent_phy_used_ = seastar::net::ntoh(be_extent_phy_used);

    uint32_t be_extent_logic_used;
    if (!read_data(&be_extent_logic_used, sizeof(be_extent_logic_used))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }
    extent_logic_used_ = seastar::net::ntoh(be_extent_logic_used);

    if (!read_data(&redundant_type_, sizeof(redundant_type_))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }

    if (!read_data(&sealed_, sizeof(sealed_))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }

    if (!read_data(&is_migrating_, sizeof(is_migrating_))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }

    if (!read_data(&corrupted_, sizeof(corrupted_))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }

    if (!read_data(&reserved_, sizeof(reserved_))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }

    // footer deserialize
    uint32_t be_footer_magic;
    if (!read_data(&be_footer_magic, sizeof(be_footer_magic))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }
    block_footer_.magic_ = seastar::net::ntoh(be_footer_magic);
    if (block_footer_.magic_ != k_block_magic) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_MAGIC_ILLEGAL;
    }

    if (!read_data(&block_footer_.version_, sizeof(block_footer_.version_))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }

    if (!read_data(&block_footer_.flag_, sizeof(block_footer_.flag_))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }

    uint16_t be_length;
    if (!read_data(&be_length, sizeof(be_length))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }
    block_footer_.length_ = seastar::net::ntoh(be_length);

    uint64_t be_footer_extent_id;
    if (!read_data(&be_footer_extent_id, sizeof(be_footer_extent_id))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }
    block_footer_.extent_id_ = seastar::net::ntoh(be_footer_extent_id);

    if (!read_data(&block_footer_.reserved_, sizeof(block_footer_.reserved_))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }

    uint32_t be_crc;
    if (!read_data(&be_crc, sizeof(be_crc))) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_DATA_ILLEGAL;
    }

    block_footer_.crc_ = seastar::net::ntoh(be_crc);

    if (const uint32_t calculated_crc = CRC::CRC32(data.data(), ptr - data.data() - sizeof(block_footer_.crc_));
        calculated_crc != block_footer_.crc_) {
        return ErrorCode::DATANODE_STORAGE_BLOCK_CRC_MISMATCH;
    }

    return ErrorCode::PANCAKE_STORE_OK;
}

} //namespace pancake_store::datanode::storage