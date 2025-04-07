
#pragma once

#include "src/comm/error_code.h"
#include <seastar/core/seastar.hh>

namespace pancake_store::datanode::comm {

using pancake_store::comm::ErrorCode;

class DeviceFunctions {
public:
    DeviceFunctions() = default;
    ~DeviceFunctions() = default;

    static std::pair<ErrorCode, seastar::sstring> GetDevicePathByUUID(const seastar::sstring &uuid);
    static std::pair<ErrorCode, bool> IsDeviceMounted(const seastar::sstring &path);
    static std::pair<ErrorCode, uint64_t> GetDeviceSize(const seastar::sstring &device_name);
    static std::pair<ErrorCode, uint64_t> GetDeviceBlockSize(const seastar::sstring &device_name);
    static std::pair<ErrorCode, uint64_t> GetDeviceBlockCount(const seastar::sstring &device_name);
    static std::pair<ErrorCode, seastar::sstring> GetDeviceUUID(const seastar::sstring &path);
};

} // namespace pancake_store::datanode::comm