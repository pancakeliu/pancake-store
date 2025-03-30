
#include "src/datanode/service/device_service.h"
#include "src/rpc/serializer.h"

#include <seastar/core/sleep.hh>

namespace pancake_store::datanode::service {

using pancake_store::rpc::Serializer;

seastar::future<> DeviceService::Start(uint16_t listen_port) {
    logger_.info("Starting device service");

    rpc_server_.RegisterAddDeviceHandler(addDevice);
    rpc_server_.RegisterDelDeviceHandler(delDevice);

    auto server = std::make_unique<seastar::rpc::protocol<Serializer>::server>(rpc_server_.GetRpc(), seastar::socket_address(seastar::ipv4_addr{"0.0.0.0", listen_port}));
    return seastar::keep_doing([this] {
        return seastar::sleep(std::chrono::milliseconds(10));
    }).finally([this, &server] {
        return server->stop();
    });
}

seastar::future<ErrorCode> DeviceService::addDevice(const AddDeviceRequest& request, AddDeviceResponse* response) {
    Instance().logger_.info("add device invoke success...");
    return seastar::make_ready_future<ErrorCode>(ErrorCode::PANCAKE_STORE_OK);
}

seastar::future<ErrorCode> DeviceService::delDevice(const DelDeviceRequest& request, DelDeviceResponse* response) {
    Instance().logger_.info("del device invoke success...");
    return seastar::make_ready_future<ErrorCode>(ErrorCode::PANCAKE_STORE_OK);
}

} // namespace pancake_store::datanode::service