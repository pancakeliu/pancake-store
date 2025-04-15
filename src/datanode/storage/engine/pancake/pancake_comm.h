
#pragma once

#include <cstddef>
#include <cstdint>

#include "src/comm/alias.h"

namespace pancake_store::datanode::storage {

using pancake_store::comm::DeviceOffset;

// NOTICE: k_data_size + k_footer_size = 4096 = 4KB
constexpr int k_block_data_size = 4064;
constexpr int k_block_footer_size = 32;
constexpr int k_block_size = k_block_data_size + k_block_footer_size;

constexpr int k_extent_block_size = 64 * 1024 * 1024;
constexpr uint64_t k_min_device_size = static_cast<const uint64_t>(k_extent_block_size * 2);

class PancakeComm {
public:
    static inline bool IsBlockAlign(DeviceOffset val);
};

} // namespace pancake_store::datanode::storage