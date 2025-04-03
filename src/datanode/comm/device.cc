
#include "src/datanode/comm/device.h"

#include <fmt/core.h>
#include <unistd.h>

namespace pancake_store::datanode::comm {

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

} // namespace pancake_store::datanode::comm