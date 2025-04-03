
#pragma once

#include "src/datanode/storage/engine/engine.h"

namespace pancake_store::datanode::storage {

// NOTICE: k_data_size + k_footer_size = 4096 = 4KB
constexpr int k_block_data_size = 4064;
constexpr int k_block_footer_size = 32;
constexpr int k_block_size = k_block_data_size + k_block_footer_size;

class PancakeFS {
public:
    struct SuperBlock {
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
    };

    using SuperBlockBackup = SuperBlock;

    struct BlockFooter {
        uint32_t magic_;
        uint8_t version_;
        uint8_t flag_; // not used
        uint16_t length_;
        uint64_t extent_id_;
        uint32_t crc_;
        uint8_t reserved_[12];
    };

    static_assert(sizeof(BlockFooter) == k_block_footer_size);

    struct JournalBlockFooter {
        uint32_t magic_;
        uint8_t version_;
        uint8_t flag_; // not used
        uint16_t length_;
        uint64_t extent_id_;
        uint32_t crc_;
        uint8_t  commited_flag_;
        uint8_t reserved_[3];
        uint64_t transaction_id_;
    };

    static_assert(sizeof(JournalBlockFooter) == sizeof(BlockFooter));

    struct Block {
        char data_[k_block_data_size];
        BlockFooter footer_;
    };

    static_assert(sizeof(Block) == k_block_size);

    struct JournalBlock {
        char old_data_[k_block_data_size];
        JournalBlockFooter journal_footer_;
    };

    static_assert(sizeof(JournalBlock) == k_block_size);

    struct ExtentHeaderBlock {
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
        uint8_t reserved_[4032];

        BlockFooter block_footer_;
    };

    static_assert(sizeof(ExtentHeaderBlock) == k_block_size);
};

} // namespace pancake_store::datanode::storage

