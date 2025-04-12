
#pragma once

#include <cstdint>

namespace pancake_store::datanode::storage {

// NOTICE: k_data_size + k_footer_size = 4096 = 4KB
constexpr int k_block_data_size = 4064;
constexpr int k_block_footer_size = 32;
constexpr int k_block_size = k_block_data_size + k_block_footer_size;

class PancakeComm {
public:
    static inline bool IsBlockAlign(uint64_t val);
};

} // namespace pancake_store::datanode::storage