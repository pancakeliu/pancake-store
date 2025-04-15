
#pragma once

#include "src/datanode/storage/engine/engine.h"
#include "src/datanode/storage/engine/pancake/pancake_io.h"
#include "src/datanode/storage/engine/pancake/pancake_comm.h"

#include "proto/device.pb.h"

#include <seastar/util/log.hh>

namespace pancake_store::datanode::storage {

using proto::device::DeviceStatus;
using proto::device::DeviceType;

class Pancake : public AbstractEngine {
public:
    Pancake() = default;
    ~Pancake() override = default;

    ErrorCode MakeFS(const MakeFSRequest &request, MakeFSResponse *response) override;
    ErrorCode Mount(const MountRequest &request, MountResponse *response) override;

    ErrorCode ReadAt(ExtentId ext_id, uint32_t offset, uint32_t size) override;
    ErrorCode WriteAt(ExtentId ext_id, const seastar::sstring &data, uint32_t offset) override;
    ErrorCode AddExtent(ExtentInfo ext_info) override;
    ErrorCode DelExtent(ExtentId ext_id) override;
    ErrorCode RecoverExtent(ExtentId ext_id) override;

    FSType GetFSType() override;
    uint64_t DeviceId() override;
    std::string DeviceUUID() override;
    std::string DevicePath() override;
    uint64_t GetTotalCapacity() override;
    uint64_t GetFreeCapacity() override;
    uint64_t GetUsedCapacity() override;
    uint64_t GetAllocateCapacity() override;

private:
    uint64_t device_id_{0};
    seastar::sstring device_uuid_;
    seastar::sstring device_path_;
    DeviceType device_type_{DeviceType::DEVICE_TYPE_UNKNOWN};

    uint64_t device_capacity_{0};
    uint64_t block_size_{k_block_data_size}; // 4k

    std::shared_ptr<PancakeIO> pancake_io_;

    seastar::logger logger_{"pancake_engine"};
};

} // namespace pancake_store::datanode::storage