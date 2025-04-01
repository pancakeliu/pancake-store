
#include "src/datanode/service/manager_service.h"
#include "src/rpc/serializer.h"

#include <seastar/core/sleep.hh>

namespace pancake_store::datanode::service {

using pancake_store::rpc::Serializer;

seastar::future<> ManagerService::Start(uint16_t listen_port) {
    logger_.info("Starting manager service");

    rpc_server_.RegisterServicePtr(this);
    rpc_server_.RegisterAddDeviceHandler(addDevice);
    rpc_server_.RegisterDelDeviceHandler(delDevice);

    auto server = std::make_unique<seastar::rpc::protocol<Serializer>::server>(rpc_server_.GetRpc(), seastar::socket_address(seastar::ipv4_addr{"0.0.0.0", listen_port}));
    return seastar::do_with(std::move(server), [this](auto &server) {
        return seastar::keep_doing([this] {
            return seastar::sleep(std::chrono::seconds(1));
        });
    });
}

seastar::future<ErrorCode> ManagerService::addDevice(void *ptr, const AddDeviceRequest& request, AddDeviceResponse* response) {
    auto *this_ptr = static_cast<ManagerService *>(ptr);
    this_ptr->logger_.info("add device. request:{}", request.ShortDebugString());

    // check cluster id
    // TODO: into config
    if (request.request_common().cluster_id() != Instance().cluster_id_) {
        this_ptr->logger_.error("cluster id mismatch. req.cluster:{}", request.request_common().cluster_id());
        return seastar::make_ready_future<ErrorCode>(ErrorCode::PANCAKE_STORE_CLUSTER_ID_NOT_MATCH);
    }
    // check datanode id

    return seastar::with_lock(this_ptr->mutex_, [this_ptr, &request, response] {
        auto iter = this_ptr->data_services_.find(request.disk_id());
        if (iter != this_ptr->data_services_.end()) {
            this_ptr->logger_.error("device:{} already exists.", request.disk_id());
            return seastar::make_ready_future<ErrorCode>(ErrorCode::PANCAKE_STORE_CLUSTER_ID_NOT_MATCH);
        }

        // TODO: codes.
        this_ptr->logger_.info("start to init device:{}", request.disk_id());

        return seastar::make_ready_future<ErrorCode>(ErrorCode::PANCAKE_STORE_OK);
    });
}

seastar::future<ErrorCode> ManagerService::delDevice(void *this_ptr, const DelDeviceRequest& request, DelDeviceResponse* response) {
    Instance().logger_.info("del device invoke success...");
    return seastar::make_ready_future<ErrorCode>(ErrorCode::PANCAKE_STORE_OK);
}

} // namespace pancake_store::datanode::service