
#pragma once

#include "src/comm/non_copyable.h"
#include "src/comm/error_code.h"
#include "src/comm/alias.h"

#include <seastar/core/seastar.hh>

namespace pancake_store::datanode::storage {

using pancake_store::comm::DeviceId;
using pancake_store::comm::NonCopyable;
using pancake_store::comm::ErrorCode;

class PancakeIO : public NonCopyable {
public:
    PancakeIO() = default;
    ~PancakeIO();

    ErrorCode Init(const seastar::sstring &device_uuid_);

private:
    seastar::sstring device_uuid_;
    seastar::sstring device_path_;
    DeviceId device_id_{0};

};

} // namespace pancake_store::datanode::storage