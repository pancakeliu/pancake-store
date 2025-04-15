
#include "src/datanode/storage/engine/pancake/pancake.h"
#include "src/datanode/storage/engine/pancake/pancake_comm.h"
#include "src/datanode/storage/engine/pancake/pancake_layout.h"
#include "src/datanode/comm/device.h"

#include <filesystem>

#include "pancake_block.h"

namespace pancake_store::datanode::storage {

using pancake_store::datanode::comm::DeviceFunctions;
using pancake_store::comm::IsNotOk;
using pancake_store::comm::ErrorCodeName;

ErrorCode Pancake::MakeFS(const MakeFSRequest &request, MakeFSResponse *response) {
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

    // check device size
    if (device_size_res.second < k_min_device_size) {
        logger_.error("device:{} device_size:{} less than min_device_size:{}, dev_path:{}",
            device_name, device_size_res.second, k_min_device_size, request.device_path_);
        return ErrorCode::DATANODE_STORAGE_DEVICE_CAPACITY_TOO_SMALL;
    }

    device_capacity_ = device_size_res.second;

    // init pancake io
    PancakeLayout pancake_layout{};
    pancake_layout.BuildLayout(device_size_res.second);

    // init tmp pancake io
    PancakeIO pancake_io{};
    auto err_code = co_await pancake_io.Init(request.device_path_, request.device_id_);
    if (IsNotOk(err_code)) {
        logger_.error("pancake io init failed. err:{}, dev_path:{}, dev_id:{}",
            ErrorCodeName(err_code), request.device_path_, request.device_id_);
        return err_code;
    }

    // init extent blocks
    auto extent_blocks_layout = pancake_layout.ExtentsBlocksLayout();
    for (auto i = 0; i < extent_blocks_layout.block_count_; ++i) {
        uint64_t dev_offset = (extent_blocks_layout.block_index_ * k_block_size) +
            static_cast<uint64_t>(k_extent_block_size * i);
        //
    }

    // init extent bit map blocks

    // init super block

    return ErrorCode::PANCAKE_STORE_OK;
}

ErrorCode Pancake::Mount(const MountRequest &request, MountResponse *response) {

}

ErrorCode Pancake::ReadAt(ExtentId ext_id, uint32_t offset, uint32_t size) {

}

ErrorCode Pancake::WriteAt(ExtentId ext_id, const seastar::sstring &data, uint32_t offset) {

}

ErrorCode Pancake::AddExtent(ExtentInfo ext_info) {

}

ErrorCode Pancake::DelExtent(ExtentId ext_id) {

}

ErrorCode Pancake::RecoverExtent(ExtentId ext_id) {

}

FSType Pancake::GetFSType() {
    return FSType::FS_TYPE_PANCAKE_BY_RAW;
}

uint64_t Pancake::DeviceId() {
    return device_id_;
}

std::string Pancake::DeviceUUID() {
    return device_uuid_;
}

std::string Pancake::DevicePath() {
    return device_path_;
}

uint64_t Pancake::GetTotalCapacity() {
    return
}

uint64_t Pancake::GetFreeCapacity() {

}

uint64_t Pancake::GetUsedCapacity() {

}

uint64_t Pancake::GetAllocateCapacity() {

}

} // namespace pancake_store::datanode::storage