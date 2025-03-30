
#include "src/rpc/datanode.h"

#include <utility>

namespace pancake_store::rpc {

using proto::service::datanode::DatanodeRpc;

void DatanodeDeviceServer::RegisterAddDeviceHandler(AddDeviceHandler handler) {
    add_device_handler_ = std::move(handler);

    rpc_.register_handler(DatanodeRpc::DATANODE_RPC_ADD_DEVICE,
        [this](seastar::sstring request_data) {
            AddDeviceRequest request{};
            AddDeviceResponse response{};
            request.ParseFromString(request_data.c_str());

            auto err_code = add_device_handler_(request, &response).get0();
            if (comm::IsNotOk(err_code)) {
                logger_.error("add_device failed. err:{}", comm::ErrorCodeName(err_code));
                response.mutable_response_common()->set_error_code(err_code);
            }

            // TODO(pancake): time cost

            return seastar::make_ready_future<seastar::sstring>(response.SerializeAsString());
        });
}

void DatanodeDeviceServer::RegisterDelDeviceHandler(DelDeviceHandler handler) {
    del_device_handler_ = std::move(handler);

    rpc_.register_handler(DatanodeRpc::DATANODE_RPC_DEL_DEVICE,
        [this](seastar::sstring request_data) {
            DelDeviceRequest request{};
            DelDeviceResponse response{};
            request.ParseFromString(request_data.c_str());

            auto err_code = del_device_handler_(request, &response).get0();
            if (comm::IsNotOk(err_code)) {
                logger_.error("del_device failed. err:{}", comm::ErrorCodeName(err_code));
                response.mutable_response_common()->set_error_code(err_code);
            }

            // TODO(pancake): time cost

            return seastar::make_ready_future<seastar::sstring>(response.SerializeAsString());
        });
}

auto DatanodeDeviceClient::AddDeviceCaller() {
    return rpc_.make_client<seastar::sstring(seastar::sstring)>(DatanodeRpc::DATANODE_RPC_ADD_DEVICE);
}

auto DatanodeDeviceClient::DelDeviceCaller() {
    return rpc_.make_client<seastar::sstring(seastar::sstring)>(DatanodeRpc::DATANODE_RPC_DEL_DEVICE);
}

void DatanodeExtentServer::RegisterAddExtentHandler(AddExtentHandler handler) {
    add_extent_handler_ = std::move(handler);

    rpc_.register_handler(DatanodeRpc::DATANODE_RPC_ADD_EXTENT,
        [this](seastar::sstring request_data) {
            AddExtentRequest request{};
            AddExtentResponse response{};
            request.ParseFromString(request_data.c_str());

            auto err_code = add_extent_handler_(request, &response);
            if (comm::IsNotOk(err_code)) {
                logger_.error("add_extent failed. err:{}", comm::ErrorCodeName(err_code));
                response.mutable_response_common()->set_error_code(err_code);
            }

            // TODO(pancake): time cost

            return seastar::make_ready_future<seastar::sstring>(response.SerializeAsString());
        });
}

void DatanodeExtentServer::RegisterDelExtentHandler(DelExtentHandler handler) {
    del_extent_handler_ = std::move(handler);

    rpc_.register_handler(DatanodeRpc::DATANODE_RPC_DEL_EXTENT,
        [this](seastar::sstring request_data) {
            DelExtentRequest request{};
            DelExtentResponse response{};
            request.ParseFromString(request_data.c_str());

            auto err_code = del_extent_handler_(request, &response);
            if (comm::IsNotOk(err_code)) {
                logger_.error("del_extent failed. err:{}", comm::ErrorCodeName(err_code));
                response.mutable_response_common()->set_error_code(err_code);
            }

            // TODO(pancake): time cost

            return seastar::make_ready_future<seastar::sstring>(response.SerializeAsString());
        });
}

void DatanodeExtentServer::RegisterSealExtentHandler(SealExtentHandler handler) {
    seal_extent_handler_ = std::move(handler);

    rpc_.register_handler(DatanodeRpc::DATANODE_RPC_SEAL_EXTENT,
        [this](seastar::sstring request_data) {
            SealExtentRequest request{};
            SealExtentResponse response{};
            request.ParseFromString(request_data.c_str());

            auto err_code = seal_extent_handler_(request, &response);
            if (comm::IsNotOk(err_code)) {
                logger_.error("seal_extent failed. err:{}", comm::ErrorCodeName(err_code));
                response.mutable_response_common()->set_error_code(err_code);
            }

            // TODO(pancake): time cost

            return seastar::make_ready_future<seastar::sstring>(response.SerializeAsString());
        });
}

void DatanodeExtentServer::RegisterListExtentsHandler(ListExtentHandler handler) {
    list_extents_handler_ = std::move(handler);

    rpc_.register_handler(DatanodeRpc::DATANODE_RPC_LIST_EXTENTS,
        [this](seastar::sstring request_data) {
            ListExtentsRequest request{};
            ListExtentsResponse response{};
            request.ParseFromString(request_data.c_str());

            auto err_code = list_extents_handler_(request, &response);
            if (comm::IsNotOk(err_code)) {
                logger_.error("list_extents failed. err:{}", comm::ErrorCodeName(err_code));
                response.mutable_response_common()->set_error_code(err_code);
            }

            // TODO(pancake): time cost

            return seastar::make_ready_future<seastar::sstring>(response.SerializeAsString());
        });
}

auto DatanodeExtentClient::AddExtentCaller() {
    return rpc_.make_client<seastar::sstring(seastar::sstring)>(DatanodeRpc::DATANODE_RPC_ADD_EXTENT);
}

auto DatanodeExtentClient::DelExtentCaller() {
    return rpc_.make_client<seastar::sstring(seastar::sstring)>(DatanodeRpc::DATANODE_RPC_DEL_EXTENT);
}

auto DatanodeExtentClient::SealExtentCaller() {
    return rpc_.make_client<seastar::sstring(seastar::sstring)>(DatanodeRpc::DATANODE_RPC_SEAL_EXTENT);
}

auto DatanodeExtentClient::ListExtentCaller() {
    return rpc_.make_client<seastar::sstring(seastar::sstring)>(DatanodeRpc::DATANODE_RPC_LIST_EXTENTS);
}

void DatanodeRwServer::RegisterWriteAtHandler(WriteAtHandler handler) {
    write_at_handler_ = std::move(handler);

    rpc_.register_handler(DatanodeRpc::DATANODE_RPC_WRITE_AT,
        [this](seastar::sstring request_data) {
            WriteAtRequest request{};
            WriteAtResponse response{};
            request.ParseFromString(request_data.c_str());

            auto err_code = write_at_handler_(request, &response);
            if (comm::IsNotOk(err_code)) {
                logger_.error("write_at failed. err:{}", comm::ErrorCodeName(err_code));
                response.mutable_response_common()->set_error_code(err_code);
            }

            // TODO(pancake): time cost

            return seastar::make_ready_future<seastar::sstring>(response.SerializeAsString());
        });
}

void DatanodeRwServer::RegisterReadAtHandler(ReadAtHandler handler) {
    read_at_handler_ = std::move(handler);
    rpc_.register_handler(DatanodeRpc::DATANODE_RPC_READ_AT,
        [this](seastar::sstring request_data) {
            ReadAtRequest request{};
            ReadAtResponse response{};
            request.ParseFromString(request_data.c_str());

            auto err_code = read_at_handler_(request, &response);
            if (comm::IsNotOk(err_code)) {
                logger_.error("read_at failed. err:{}", comm::ErrorCodeName(err_code));
                response.mutable_response_common()->set_error_code(err_code);
            }

            // TODO(pancake): time cost

            return seastar::make_ready_future<seastar::sstring>(response.SerializeAsString());
        });
}

auto DatanodeRwClient::WriteAtCaller() {
    return rpc_.make_client<seastar::sstring(seastar::sstring)>(DatanodeRpc::DATANODE_RPC_WRITE_AT);
}

auto DatanodeRwClient::ReadAtCaller() {
    return rpc_.make_client<seastar::sstring(seastar::sstring)>(DatanodeRpc::DATANODE_RPC_READ_AT);
}

} // namespace pancake_store::rpc