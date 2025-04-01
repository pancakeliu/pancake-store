
#include "src/pkg/rpc_caller/datanode_device_caller.h"

namespace pancake_store::pkg::rpc_caller {

using proto::service::datanode::DatanodeRpc;

DatanodeDeviceCaller::DatanodeDeviceCaller(const seastar::sstring &ip, const uint16_t port) {
    client_ = std::make_shared<seastar::rpc::protocol<Serializer>::client>(rpc_, seastar::ipv4_addr{ip, port});
}

DatanodeDeviceCaller::~DatanodeDeviceCaller(){
    if (!closed_) {
        logger_.error("code error!!, datanode device caller not close");
        // TODO(pancake): de-terminate???
        std::terminate();
    }
}

seastar::future<> DatanodeDeviceCaller::Close() {
    return client_->stop()
        .then([this] {
            closed_ = true;
            return seastar::make_ready_future<>();
        });
}

seastar::future<ErrorCode> DatanodeDeviceCaller::AddDevice(const AddDeviceRequest &request, AddDeviceResponse *response) {
    auto caller = ADD_DEVICE_CALLER(rpc_);

    return caller(*client_, request.SerializeAsString())
    .then([this, response](const seastar::sstring& resp_str) {
        auto ret = response->ParseFromString(resp_str);
        if (ret != 0) {
            logger_.error("Failed to parse add_device response. ret:{}", ret);
            return seastar::make_ready_future<ErrorCode>(ErrorCode::RPC_CALLER_RESPONSE_PARSE_FAILED);
        }

        return seastar::make_ready_future<ErrorCode>(response->response_common().error_code());
    })
    .handle_exception([this](auto ep) {
         logger_.error("add_device handle exception:{}", ep);
         return seastar::make_ready_future<ErrorCode>(ErrorCode::RPC_CALLER_INVOKE_FAILED);
    });
}

seastar::future<ErrorCode> DatanodeDeviceCaller::DelDevice(const DelDeviceRequest &request, DelDeviceResponse *response) {
    auto caller = DEL_DEVICE_CALLER(rpc_);

    return caller(*client_, request.SerializeAsString())
    .then([this, response](const seastar::sstring& resp_str) {
        auto ret = response->ParseFromString(resp_str);
        if (ret != 0) {
            logger_.error("Failed to parse del_device response. ret:{}", ret);
            return seastar::make_ready_future<ErrorCode>(ErrorCode::RPC_CALLER_RESPONSE_PARSE_FAILED);
        }

        return seastar::make_ready_future<ErrorCode>(response->response_common().error_code());
    })
    .handle_exception([this](auto ep) {
         logger_.error("del_device handle exception:{}", ep);
         return seastar::make_ready_future<ErrorCode>(ErrorCode::RPC_CALLER_INVOKE_FAILED);
    });
}

} // namespace pancake_store::pkg::rpc_caller