#pragma once

#include "src/comm/non_copyable.h"
#include "src/comm/error_code.h"
#include "src/rpc/datanode.h"

#include <seastar/rpc/rpc.hh>

namespace pancake_store::datanode::service {

using pancake_store::comm::NonCopyable;
using pancake_store::comm::ErrorCode;

using pancake_store::rpc::DatanodeDeviceServer;

using proto::service::datanode::AddDeviceRequest;
using proto::service::datanode::AddDeviceResponse;
using proto::service::datanode::DelDeviceRequest;
using proto::service::datanode::DelDeviceResponse;

class DeviceService : public NonCopyable {
public:
    static DeviceService &Instance() {
        static DeviceService instance;
        return instance;
    }

    seastar::future<> Start(uint16_t listen_port);

private:
    DeviceService() = default;
    ~DeviceService() = default;

    static seastar::future<ErrorCode> addDevice(const AddDeviceRequest& request, AddDeviceResponse* response);
    static seastar::future<ErrorCode> delDevice(const DelDeviceRequest& request, DelDeviceResponse* response);

private:
    DatanodeDeviceServer rpc_server_;
    seastar::logger logger_{"device_service"};
};

} // namespace pancake_store::datanode::service