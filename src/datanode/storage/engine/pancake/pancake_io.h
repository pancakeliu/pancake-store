
#pragma once

#include "src/comm/non_copyable.h"
#include "src/comm/error_code.h"
#include "src/comm/alias.h"

#include <seastar/core/seastar.hh>
#include <seastar/util/log.hh>

namespace pancake_store::datanode::storage {

using pancake_store::comm::DeviceId;
using pancake_store::comm::NonCopyable;
using pancake_store::comm::ErrorCode;

class PancakeIO : public NonCopyable {
public:
    PancakeIO() = default;
    ~PancakeIO();

    seastar::future<ErrorCode> Init(const seastar::sstring &device_path, DeviceId device_id);

private:
    seastar::sstring device_path_;
    DeviceId device_id_{0};

    std::shared_ptr<seastar::file> device_fd_;
    seastar::logger logger_{"pancake_io"};
};

} // namespace pancake_store::datanode::storage