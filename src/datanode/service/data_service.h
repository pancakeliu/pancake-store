
#pragma once

#include "src/comm/alias.h"
#include "src/comm/error_code.h"
#include "src/comm/non_copyable.h"

namespace pancake_store::datanode::service {

using pancake_store::comm::NonCopyable;
using pancake_store::comm::ErrorCode;
using pancake_store::comm::DeviceId;

class DataService : public NonCopyable {
public:
    DataService(DeviceId device_id, uint16_t listen_port) {}
};

} // namespace pancake_store::datanode::service