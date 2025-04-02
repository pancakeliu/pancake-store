
#pragma once

#include <seastar/core/seastar.hh>


#include "src/comm/error_code.h"
#include "src/comm/alias.h"

#include "proto/device.pb.h"
#include "proto/extent.pb.h"

namespace pancake_store::datanode::storage {

using pancake_store::comm::ExtentId;
using pancake_store::comm::ErrorCode;

using proto::extent::ExtentInfo;
using proto::device::DeviceInfo;
using proto::device::FSType;

class AbstractEngine {
public:
    virtual ~AbstractEngine() = default;
    virtual ErrorCode Init(DeviceInfo device_info) = 0;
    virtual ErrorCode ReadAt(ExtentId ext_id, uint32_t offset, uint32_t size) = 0;
    virtual ErrorCode WriteAt(ExtentId ext_id, const seastar::sstring &data, uint32_t offset) = 0;
    virtual ErrorCode AddExtent(ExtentInfo ext_info);
    virtual ErrorCode DelExtent(ExtentId ext_id) = 0;
    virtual ErrorCode RecoverExtent(ExtentId ext_id) = 0;

    virtual FSType GetFSType() = 0;
    virtual uint64_t DeviceId() = 0;
    virtual std::string DeviceUUID() = 0;
    virtual std::string DevicePath() = 0;
    virtual uint64_t GetTotalCapacity() = 0;
    virtual uint64_t GetFreeCapacity() = 0;
    virtual uint64_t GetUsedCapacity() = 0;
    virtual uint64_t GetAllocateCapactity() = 0;
};

}  // namespace pancake_store::datanode::storage

