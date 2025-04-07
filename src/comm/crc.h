
#pragma once

#include <cstdint>

namespace pancake_store::comm {

class CRC {
public:
    static uint32_t CRC32(const char *data, size_t size);
};

} // namespace pancake_store::comm