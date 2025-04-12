
#pragma once

#include "src/datanode/storage/engine/pancake/pancake_fs_block.h"
#include "src/datanode/storage/engine/pancake/pancake_io.h"
#include "src/comm/non_copyable.h"

#include <seastar/core/shared_mutex.hh>

namespace pancake_store::datanode::storage {

using pancake_store::comm::NonCopyable;

class ExtentBitMap {
public:
    explicit ExtentBitMap(const std::shared_ptr<PancakeIO>& pancake_io) : pancake_io_{pancake_io} {}
    ~ExtentBitMap() = default;

    ErrorCode Init(uint64_t offset, uint32_t bitmap_block_count, uint32_t extent_block_count,
                   uint32_t extent_block_size, uint64_t extent_blocks_offset);

    seastar::future<ErrorCode> AddExtent(const ExtentHeaderBlock &ext_header);
    seastar::future<ErrorCode> DelExtent(const ExtentHeaderBlock &ext_header);

    seastar::future<uint32_t> CurrentAllocCount();
    seastar::future<uint32_t> CurrentFreeCount();

private:


private:
    std::shared_ptr<PancakeIO> pancake_io_;

    uint64_t offset_{0};
    uint32_t bitmap_block_count_{0};
    uint32_t extent_block_count_{0};
    uint32_t extent_block_size_{0};
    uint32_t extent_blocks_offset_{0};

    std::vector<bool> bitmap_;
    uint32_t cursor_{0};
    uint32_t used_cnt_{0};
    seastar::shared_mutex mutex_;

    seastar::logger logger_{"pancake_fs::extent_bitmap"};
};

class PancakeFS : public NonCopyable {
public:
    PancakeFS() = default;
    ~PancakeFS() = default;

private:
    uint64_t device_id_{0};
    seastar::sstring device_uuid_;
    seastar::sstring device_path_;

    uint64_t device_capacity_{0};
    uint64_t block_size_{k_block_data_size}; // 4k

    ExtentBitMap extent_bitmap_;

    seastar::logger logger_{"pancake_fs::pancake_fs"};
};


} // namespace pancake_store::datanode::storage

