
#pragma once

#include <src/comm/non_copyable.h>


#include <seastar/core/seastar.hh>


#include "src/comm/error_code.h"
#include "src/comm/alias.h"

#include "proto/device.pb.h"
#include "proto/extent.pb.h"

namespace pancake_store::datanode::storage {

using pancake_store::comm::ExtentId;
using pancake_store::comm::ErrorCode;
using pancake_store::comm::NonCopyable;

using proto::extent::ExtentInfo;
using proto::device::FSType;
using proto::device::DeviceType;
using proto::device::DeviceBaseInfo;

constexpr uint32_t k_magic_id = 20250430;

struct MakeFSRequest {
    uint64_t device_id_;
    seastar::sstring device_path_;
    FSType fs_type_;
};

struct MakeFSResponse {
    seastar::sstring device_uuid_;
    uint64_t total_capacity_;
};

struct MountRequest {
    uint64_t device_id_;
    seastar::sstring device_path_; // NOTICE: only used by posix fs
    seastar::sstring device_uuid_;
    FSType fs_type_;
    uint64_t total_capacity_;
};

struct MountResponse {};

class AbstractEngine {
public:
    virtual ~AbstractEngine() = default;

    virtual ErrorCode MakeFS(const MakeFSRequest &request, MakeFSResponse *response) = 0;
    virtual ErrorCode Mount(const MountRequest &request, MountResponse *response) = 0;

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
    virtual uint64_t GetAllocateCapacity() = 0;
};

}  // namespace pancake_store::datanode::storage

