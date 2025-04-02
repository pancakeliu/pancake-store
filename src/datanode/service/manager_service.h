#pragma once

#include "src/comm/non_copyable.h"
#include "src/comm/error_code.h"
#include "src/comm/alias.h"
#include "src/rpc/datanode.h"

#include <seastar/rpc/rpc.hh>
#include <seastar/core/shared_mutex.hh>

#include <cstdint>

namespace pancake_store::datanode::service {

using pancake_store::comm::NonCopyable;
using pancake_store::comm::ErrorCode;
using pancake_store::comm::DeviceId;

using pancake_store::rpc::DatanodeDeviceServer;

using proto::service::datanode::AddDeviceRequest;
using proto::service::datanode::AddDeviceResponse;
using proto::service::datanode::DelDeviceRequest;
using proto::service::datanode::DelDeviceResponse;

class ManagerService : public NonCopyable {
public:
    static ManagerService &Instance() {
        static ManagerService instance;
        return instance;
    }

    // TODO(pancake): init, load all device info from master
    seastar::future<> Start(uint16_t listen_port);

private:
    ManagerService() = default;
    ~ManagerService() = default;

    static seastar::future<ErrorCode> addDevice(void *ptr, const AddDeviceRequest& request, AddDeviceResponse* response);
    static seastar::future<ErrorCode> delDevice(void *ptr, const DelDeviceRequest& request, DelDeviceResponse* response);

private:
    DatanodeDeviceServer rpc_server_{};
    seastar::logger logger_{"manager_service"};

    std::unordered_map<DeviceId, std::shared_ptr<ManagerService>> data_services_;
    seastar::shared_mutex mutex_;
};

} // namespace pancake_store::datanode::service