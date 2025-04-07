
#include "src/datanode/comm/device.h"

#include <fmt/core.h>
#include <unistd.h>
#include <fstream>
#include <filesystem>

namespace pancake_store::datanode::comm {

using pancake_store::comm::IsNotOk;

constexpr int k_path_max = 4096;

std::pair<ErrorCode, seastar::sstring> DeviceFunctions::GetDevicePathByUUID(const seastar::sstring &uuid) {
    char real_path[k_path_max];
    seastar::sstring uuid_path(fmt::format("/dev/disk/by-uuid/{}", uuid));
    int len = readlink(uuid_path.c_str(), real_path, k_path_max - 1);
    if (len < 0) {
        return {ErrorCode::DATANODE_DEVICE_READ_LINK_FAILED, {}};
    }
    return {ErrorCode::PANCAKE_STORE_OK, {real_path, static_cast<size_t>(len)}};
}

std::pair<ErrorCode, bool> DeviceFunctions::IsDeviceMounted(const seastar::sstring &device_path) {
    std::ifstream mounts_file("/proc/mounts");
    if (!mounts_file.is_open()) {
        return {ErrorCode::DATANODE_DEVICE_FILE_NOT_EXIST, false};
    }
    std::string line;

    while (std::getline(mounts_file, line)) {
        std::istringstream iss(line);

        std::string mounted_device;
        std::string mount_point;
        std::string filesystem;

        iss >> mounted_device >> mount_point >> filesystem;

        if (mounted_device == device_path) {
            return {ErrorCode::PANCAKE_STORE_OK, true};
        }
    }

    return {ErrorCode::PANCAKE_STORE_OK, false};
}

std::pair<ErrorCode, uint64_t> DeviceFunctions::GetDeviceSize(const seastar::sstring &device_name) {
    auto block_size_res = GetDeviceBlockSize(device_name);
    if (IsNotOk(block_size_res.first)) {
        return {block_size_res.first, 0};
    }

    auto block_cnt_res = GetDeviceBlockCount(device_name);
    if (IsNotOk(block_cnt_res.first)) {
        return {block_cnt_res.first, 0};
    }

    return {ErrorCode::PANCAKE_STORE_OK, block_size_res.second * block_cnt_res.second};
}

std::pair<ErrorCode, uint64_t> DeviceFunctions::GetDeviceBlockSize(const seastar::sstring &path) {
    seastar::sstring block_size_path = fmt::format("/sys/block/{}/queue/logical_block_size", path);

    std::ifstream size_file(block_size_path);
    if (!size_file.is_open()) {
        return {ErrorCode::DATANODE_DEVICE_FILE_NOT_EXIST, 0};
    }

    uint64_t block_size = 0;
    size_file >> block_size;

    return {ErrorCode::PANCAKE_STORE_OK, block_size};
}

std::pair<ErrorCode, uint64_t> DeviceFunctions::GetDeviceBlockCount(const seastar::sstring &device_name) {
    seastar::sstring device_size_path = fmt::format("/sys/block/{}/size", device_name);

    std::ifstream size_file(device_size_path);
    if (!size_file.is_open()) {
        return {ErrorCode::DATANODE_DEVICE_FILE_NOT_EXIST, 0};
    }

    uint64_t blocks = 0;
    size_file >> blocks;

    return {ErrorCode::PANCAKE_STORE_OK, blocks};
}

std::pair<ErrorCode, seastar::sstring> DeviceFunctions::GetDeviceUUID(const seastar::sstring &dev_path) {
    try {
        std::filesystem::path device_path = std::filesystem::canonical(device_path);

        for (const auto &entry : std::filesystem::directory_iterator("/dev/disk/by-uuid")) {
            if (std::filesystem::equivalent(entry, dev_path.c_str())) {
                return {ErrorCode::PANCAKE_STORE_OK, {entry.path().filename()}};
            }
        }
    } catch (const std::exception &e) {
        return {ErrorCode::DATANODE_DEVICE_FILE_SYSTEM_ERROR, {}};
    }

    return {ErrorCode::DATANODE_DEVICE_FILE_NOT_EXIST, {}};
}

} // namespace pancake_store::datanode::comm