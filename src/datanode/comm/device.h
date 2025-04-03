
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
};

} // namespace pancake_store::datanode::comm