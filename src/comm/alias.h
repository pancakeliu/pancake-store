#pragma once

#include <cstdint>
#include <string>

namespace pancake_store::comm {

using DeviceId = uint64_t;
using ExtentId = uint64_t;
using ExtentNodeId = std::string;

using DeviceOffset = uint64_t;

} // namespace pancake_store::comm