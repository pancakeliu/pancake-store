
#include "src/datanode/service/manager_service.h"
#include "src/rpc/serializer.h"

#include <seastar/core/sleep.hh>

namespace pancake_store::datanode::service {

using pancake_store::rpc::Serializer;

seastar::future<> ManagerService::Start(uint16_t listen_port) {
    logger_.info("Starting manager service");

    rpc_server_.RegisterAddDeviceHandler(addDevice);
    rpc_server_.RegisterDelDeviceHandler(delDevice);

    auto server = std::make_unique<seastar::rpc::protocol<Serializer>::server>(rpc_server_.GetRpc(), seastar::socket_address(seastar::ipv4_addr{"0.0.0.0", listen_port}));
    return seastar::do_with(std::move(server), [this](auto &server) {
        return seastar::keep_doing([this] {
            return seastar::sleep(std::chrono::seconds(1));
        });
    });
}

ErrorCode ManagerService::addDevice(const AddDeviceRequest& request, AddDeviceResponse* response) {
    Instance().logger_.info("add device invoke success...");
    return ErrorCode::PANCAKE_STORE_OK;
}

ErrorCode ManagerService::delDevice(const DelDeviceRequest& request, DelDeviceResponse* response) {
    Instance().logger_.info("del device invoke success...");
    return ErrorCode::PANCAKE_STORE_OK;
}

} // namespace pancake_store::datanode::service