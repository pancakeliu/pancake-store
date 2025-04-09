
#pragma once

#include "src/datanode/storage/engine/engine.h"

namespace pancake_store::datanode::storage {

class PancakeFS {
public:
    PancakeFS() = default;
    ~PancakeFS() = default;

private:
    uint64_t device_id_;
    seastar::sstring device_uuid_;
    seastar::sstring device_path_;

    uint64_t device_capacity_;
    uint64_t block_size_; // 4k
    uint32_t journal_block_count_;

    //ExtentBlockManager extent_block_manager_;

};

class ExtentBlockManager {

};

} // namespace pancake_store::datanode::storage

