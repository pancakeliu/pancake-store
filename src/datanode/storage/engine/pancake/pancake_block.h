
#pragma once

#include "src/comm/error_code.h"
#include "src/datanode/storage/engine/pancake/pancake_comm.h"

#include <seastar/core/seastar.hh>
#include <cstdint>

namespace pancake_store::datanode::storage {

using pancake_store::comm::ErrorCode;

// super block
constexpr int k_super_block_index = 0;
constexpr int k_backup_super_block_size = 1;

constexpr uint32_t k_block_magic = 0x20250430;

struct SuperBlock {
public:
    seastar::sstring Serialize();
    ErrorCode Deserialize(const seastar::sstring& data);

public:
    uint32_t magic_;
    uint8_t version_;
    uint64_t device_id_;

    seastar::sstring device_uuid_;
    seastar::sstring device_path_;

    int64_t create_time_;
    uint64_t device_capacity_;
    uint64_t block_size_; // 4k
    uint32_t journal_block_count_;

    // block bitmap
    uint64_t extent_block_size_;
    uint64_t extent_block_count_;

    // extent block bitmap
    uint64_t extent_block_bitmap_start_;
    uint64_t extent_block_bitmap_end_;
    // trash extent block bitmap
    uint64_t trash_extent_block_bitmap_start_;
    uint64_t trash_extent_block_bitmap_end_;
    // journal blocks
    uint64_t journal_blocks_start_;
    uint64_t journal_blocks_end_;

    // extent blocks
    uint64_t extent_blocks_start_;

    uint32_t crc_;
};

using SuperBlockBackup = SuperBlock;

struct BlockFooter {
    void Fill(uint64_t extent_id, uint64_t length) {
        magic_ = k_block_magic;
        version_ = 1;
        // flag_ = 0;
        length_ = length;
        extent_id_ = extent_id;
        // memset(reserved_, 0, sizeof(reserved_));
        // crc_ = 0;
    }

    void FillWithoutExtentId(uint64_t length) {
        magic_ = k_block_magic;
        version_ = 1;
        // flag_ = 0;
        length_ = length;
        extent_id_ = 0;
        // memset(reserved_, 0, sizeof(reserved_));
        // crc_ = 0;
    }

    uint32_t magic_;
    uint8_t version_;
    uint8_t flag_; // not used
    uint16_t length_;
    uint64_t extent_id_;
    uint8_t reserved_[12];
    uint32_t crc_{0};
};

static_assert(sizeof(BlockFooter) == k_block_footer_size);

struct JournalBlockFooter {
    uint32_t magic_;
    uint8_t version_;
    uint8_t flag_; // not used
    uint16_t length_;
    uint64_t extent_id_;
    uint64_t transaction_id_;
    uint8_t  commited_flag_;
    uint8_t reserved_[3];
    uint32_t crc_;
};

static_assert(sizeof(JournalBlockFooter) == sizeof(BlockFooter));

struct BitMapBlock {
public:
    seastar::sstring Serialize();
    ErrorCode Deserialize(const seastar::sstring& data);

public:
    uint8_t bitmap_[k_block_data_size];
    BlockFooter footer_;
};

static_assert(sizeof(BitMapBlock) == k_block_size);

struct DataBlock {
public:
    seastar::sstring Serialize();
    ErrorCode Deserialize(const seastar::sstring& data);

public:
    char data_[k_block_data_size];
    BlockFooter footer_;
};

static_assert(sizeof(DataBlock) == k_block_size);

struct JournalBlock {
public:
    seastar::sstring Serialize();
    ErrorCode Deserialize(const seastar::sstring& data);

public:
    char old_data_[k_block_data_size];
    JournalBlockFooter journal_footer_;
};

static_assert(sizeof(JournalBlock) == k_block_size);

struct ExtentHeaderBlock {
public:
    seastar::sstring Serialize();
    ErrorCode Deserialize(const seastar::sstring& data);

public:
    uint32_t magic_;
    uint8_t version_;
    uint8_t shard_id_;
    // redundant members
    union {
        uint16_t rep_shard_cnt_;
        struct {
            uint8_t data_shard_cnt_;
            uint8_t code_shard_cnt_;
        };
    };
    uint64_t device_id_;
    uint64_t extent_id_;
    uint32_t extent_capacity_;
    uint32_t extent_phy_used_;
    uint32_t extent_logic_used_; // used to emit metrics
    struct {
        uint8_t redundant_type_;
        uint8_t sealed_;
        uint8_t is_migrating_;
        uint8_t corrupted_;
    };
    uint32_t bitmap_ext_index_;
    uint8_t reserved_[4020];

    BlockFooter block_footer_;
};

static_assert(sizeof(ExtentHeaderBlock) == k_block_size);

} // namespace pancake_store::datanode::storage