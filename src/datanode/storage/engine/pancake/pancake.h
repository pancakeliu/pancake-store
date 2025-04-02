
#pragma once

#include "src/datanode/storage/engine/engine.h"

namespace pancake_store::datanode::storage {

class PancakeEngine : public AbstractEngine {
public:
    PancakeEngine() = default;
    ~PancakeEngine() override = default;

    ErrorCode Init(DeviceInfo device_info) override;
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
    uint64_t GetAllocateCapactity() override;

private:
    // TODO: de-DeviceInfo
    DeviceInfo device_info_;
};

} // namespace pancake_store::datanode::storage