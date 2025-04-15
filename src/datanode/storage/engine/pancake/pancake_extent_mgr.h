#pragma once

#include "src/comm/alias.h"
#include "src/comm/error_code.h"
#include "src/datanode/storage/engine/pancake/pancake_io.h"
#include "src/datanode/storage/engine/pancake/pancake_block.h"

#include <seastar/core/shared_mutex.hh>

namespace pancake_store::datanode::storage {

using pancake_store::comm::ErrorCode;
using pancake_store::comm::ExtentId;
using pancake_store::comm::DeviceOffset;

class PancakeExtentManager {
public:
    explicit PancakeExtentManager(const std::shared_ptr<PancakeIO>& pancake_io) : pancake_io_{pancake_io} {}
    ~PancakeExtentManager() = default;

    ErrorCode Init(DeviceOffset offset, uint32_t bitmap_block_count, uint32_t extent_block_count,
                   uint32_t extent_block_size, DeviceOffset extent_blocks_offset);

    seastar::future<ErrorCode> AddExtent(ExtentHeaderBlock *ext_header);
    seastar::future<ErrorCode> DelExtent(const ExtentHeaderBlock &ext_header);

    seastar::future<std::pair<ErrorCode, DeviceOffset>> ExtentDeviceOffset(ExtentId extent_id);

    seastar::future<uint32_t> CurrentAllocCount();
    seastar::future<uint32_t> CurrentFreeCount();

private:
    // NOTICE: do not need lock
    BitMapBlock cloneBitMapBlockWithoutLock(uint32_t ext_block_index);
    static void markExtentBlockUsed(BitMapBlock *bitmap_block, uint32_t ext_block_index);
    static void markExtentBlockUnused(BitMapBlock *bitmap_block, uint32_t ext_block_index);

    static inline uint32_t bitmapIndex(uint64_t extent_block_index);
    inline DeviceOffset bitmapBlockDeviceOffset(uint32_t bitmap_block_index) const;
    inline DeviceOffset extentBlockDeviceOffset(uint32_t extent_block_index) const;

private:
    std::shared_ptr<PancakeIO> pancake_io_;

    DeviceOffset offset_{0};
    uint32_t bitmap_block_count_{0};
    uint32_t extent_block_count_{0};
    uint32_t extent_block_size_{0};
    DeviceOffset extent_blocks_offset_{0};

    std::vector<bool> bitmap_;
    uint32_t cursor_{0};
    uint32_t used_cnt_{0};

    std::unordered_map<ExtentId, DeviceOffset> extent_offset_map_;

    seastar::shared_mutex mutex_;

    seastar::logger logger_{"pancake_fs::extent_manager"};
};

} // namespace pancake_store::datanode::storage