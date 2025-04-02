
#include "src/datanode/service/manager_service.h"
#include "src/datanode/comm/config.h"
#include "src/rpc/serializer.h"

#include <seastar/core/sleep.hh>

namespace pancake_store::datanode::service {

using pancake_store::datanode::comm::Config;
using pancake_store::rpc::Serializer;

seastar::future<> ManagerService::Start(uint16_t listen_port) {
    assert(seastar::this_shard_id() == main_worker_cpu_id);

    logger_.info("Starting manager service");

    rpc_server_.RegisterServicePtr(this);
    rpc_server_.RegisterAddDeviceHandler(addDevice);
    rpc_server_.RegisterDelDeviceHandler(delDevice);

    auto server = std::make_unique<seastar::rpc::protocol<Serializer>::server>(rpc_server_.GetRpc(), seastar::socket_address(seastar::ipv4_addr{"0.0.0.0", listen_port}));
    return seastar::do_with(std::move(server), [this](auto &server) {
        (void)server;
        return seastar::keep_doing([this] {
            return seastar::sleep(std::chrono::seconds(60));
        });
    });
}

seastar::future<ErrorCode> ManagerService::addDevice(void *ptr, const AddDeviceRequest& request, AddDeviceResponse* response) {
    assert(seastar::this_shard_id() == main_worker_cpu_id);

    auto *this_ptr = static_cast<ManagerService *>(ptr);
    this_ptr->logger_.info("add device. request:{}", request.ShortDebugString());

    // check cluster id
    if (request.request_common().cluster_id() != Config::Instance().ClusterId()) {
        this_ptr->logger_.error("cluster id mismatch. req.cluster:{}",
            request.request_common().cluster_id());
        return seastar::make_ready_future<ErrorCode>(ErrorCode::PANCAKE_STORE_CLUSTER_ID_NOT_MATCH);
    }
    // check datanode id
    if (request.datanode_id() != Config::Instance().DatanodeId()) {
        this_ptr->logger_.error("datanode id mismatch. req.datanode_id:{}", request.datanode_id());
        return seastar::make_ready_future<ErrorCode>(ErrorCode::DATANODE_SERVICE_DATANODE_ID_NOT_MATCH);
    }
    if (request.disk_id() == 0) {
        this_ptr->logger_.error("min disk id is 1!! request:{}", request.ShortDebugString());
        return seastar::make_ready_future<ErrorCode>(ErrorCode::DATANODE_SERVICE_REQUEST_ARGUMENT_INVALID);
    }

    return seastar::with_lock(this_ptr->mutex_, [this_ptr, &request, response] {
        auto iter = this_ptr->data_services_.find(request.disk_id());
        if (iter != this_ptr->data_services_.end()) {
            this_ptr->logger_.error("device:{} already exists.", request.disk_id());
            return seastar::make_ready_future<ErrorCode>(ErrorCode::PANCAKE_STORE_CLUSTER_ID_NOT_MATCH);
        }

        this_ptr->logger_.info("start to init device:{}, request:{}",
            request.disk_id(), request.ShortDebugString());



        return seastar::make_ready_future<ErrorCode>(ErrorCode::PANCAKE_STORE_OK);
    });
}

seastar::future<ErrorCode> ManagerService::delDevice(void *this_ptr, const DelDeviceRequest& request, DelDeviceResponse* response) {
    assert(seastar::this_shard_id() == main_worker_cpu_id);

    Instance().logger_.info("del device invoke success...");
    return seastar::make_ready_future<ErrorCode>(ErrorCode::PANCAKE_STORE_OK);
}

} // namespace pancake_store::datanode::service