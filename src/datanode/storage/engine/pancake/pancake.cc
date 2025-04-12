
#include "src/datanode/storage/engine/pancake/pancake.h"
#include "src/datanode/storage/engine/pancake/pancake_comm.h"
#include "src/datanode/comm/device.h"

#include <filesystem>

namespace pancake_store::datanode::storage {

using pancake_store::datanode::comm::DeviceFunctions;
using pancake_store::comm::IsNotOk;
using pancake_store::comm::ErrorCodeName;

ErrorCode PancakeEngine::MakeFS(const MakeFSRequest &request, MakeFSResponse *response) {
    if (request.fs_type_ != FSType::FS_TYPE_PANCAKE_BY_RAW) {
        logger_.error("code error!! fs_type not pancake.");
        return ErrorCode::PANCAKE_STORE_CODE_ERROR;
    }

    // check device status
    auto mounted_res = DeviceFunctions::IsDeviceMounted(request.device_path_);
    if (IsNotOk(mounted_res.first)) {
        logger_.error("get device mount status failed. err:{}, device:{}",
            ErrorCodeName(mounted_res.first), request.device_path_);
        return mounted_res.first;
    }
    if (mounted_res.second) {
        logger_.error("device:{} already mounted!", request.device_path_);
        return ErrorCode::DATANODE_DEVICE_ALREADY_MOUNTED;
    }

    // get device uuid
    auto get_uuid_res = DeviceFunctions::GetDeviceUUID(request.device_path_);
    if (IsNotOk(get_uuid_res.first)) {
        logger_.error("get device uuid failed. err:{}, device:{}",
            ErrorCodeName(get_uuid_res.first), request.device_path_);
        return get_uuid_res.first;
    }

    auto device_name = std::filesystem::path(request.device_path_).filename().string();

    auto device_size_res = DeviceFunctions::GetDeviceSize(device_name);
    if (IsNotOk(device_size_res.first)) {
        logger_.error("get device size failed. err:{}, device:{}, device_name:{}",
            ErrorCodeName(device_size_res.first), request.device_path_, device_name);
        return device_size_res.first;
    }

    // init pancake io


    return ErrorCode::PANCAKE_STORE_OK;
}

ErrorCode PancakeEngine::Mount(const MountRequest &request, MountResponse *response) {

}

ErrorCode PancakeEngine::ReadAt(ExtentId ext_id, uint32_t offset, uint32_t size) {

}

ErrorCode PancakeEngine::WriteAt(ExtentId ext_id, const seastar::sstring &data, uint32_t offset) {

}

ErrorCode PancakeEngine::AddExtent(ExtentInfo ext_info) {

}

ErrorCode PancakeEngine::DelExtent(ExtentId ext_id) {

}

ErrorCode PancakeEngine::RecoverExtent(ExtentId ext_id) {

}

FSType PancakeEngine::GetFSType() {

}

uint64_t PancakeEngine::DeviceId() {

}

std::string PancakeEngine::DeviceUUID() {

}

std::string PancakeEngine::DevicePath() {

}

uint64_t PancakeEngine::GetTotalCapacity() {

}

uint64_t PancakeEngine::GetFreeCapacity() {

}

uint64_t PancakeEngine::GetUsedCapacity() {

}

uint64_t PancakeEngine::GetAllocateCapacity() {

}

} // namespace pancake_store::datanode::storage