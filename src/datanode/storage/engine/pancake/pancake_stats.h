
#pragma once

#include "proto/device.pb.h"

namespace pancake_store::datanode::storage {

using proto::device::DeviceStatus;

class PancakeStats {
public:
    PancakeStats() = default;
    ~PancakeStats() = default;

private:
    DeviceStatus device_status_{DeviceStatus::DEVICE_STATUS_UNKNOWN};

    uint64_t device_capacity_{0};
    uint64_t device_used_size_{0};
    uint64_t device_allocated_size_{0};
};

} // namespace pancake_store::datanode::storage