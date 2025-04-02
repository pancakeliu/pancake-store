
#pragma once

#include "src/comm/non_copyable.h"
#include "src/datanode/storage/engine/engine.h"

namespace pancake_store::datanode::storage {

using pancake_store::comm::NonCopyable;

class PancakeCache : NonCopyable {
public:
    PancakeCache() = default;
    ~PancakeCache() = default;

    ErrorCode Init(uint64_t cache_size);

private:
    uint64_t cache_size_;
};

} // namespace pancake_store::datanode::storage