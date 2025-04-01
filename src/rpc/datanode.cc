
#include "src/rpc/datanode.h"

#include <utility>

namespace pancake_store::rpc {

using proto::service::datanode::DatanodeRpc;

void DatanodeDeviceServer::RegisterServicePtr(void *service_ptr) {
    service_ptr_ = service_ptr;
}

void DatanodeDeviceServer::RegisterAddDeviceHandler(AddDeviceHandler handler) {
    add_device_handler_ = std::move(handler);

    rpc_.register_handler(DatanodeRpc::DATANODE_RPC_ADD_DEVICE,
        [this](seastar::sstring request_data) {
            AddDeviceRequest request{};
            AddDeviceResponse response{};
            request.ParseFromString(request_data.c_str());

            return add_device_handler_(service_ptr_, request, &response)
                .then([this, &response](ErrorCode err_code) {
                    if (comm::IsNotOk(err_code)) {
                        logger_.error("add_device failed. err:{}", comm::ErrorCodeName(err_code));
                        response.mutable_response_common()->set_error_code(err_code);
                    }
                });

            // TODO(pancake): time cost
        });
}

void DatanodeDeviceServer::RegisterDelDeviceHandler(DelDeviceHandler handler) {
    del_device_handler_ = std::move(handler);

    rpc_.register_handler(DatanodeRpc::DATANODE_RPC_DEL_DEVICE,
        [this](seastar::sstring request_data) {
            DelDeviceRequest request{};
            DelDeviceResponse response{};
            request.ParseFromString(request_data.c_str());

            return del_device_handler_(service_ptr_, request, &response)
                .then([this, &response](ErrorCode err_code) {
                    if (comm::IsNotOk(err_code)) {
                        logger_.error("del_device failed. err:{}", comm::ErrorCodeName(err_code));
                        response.mutable_response_common()->set_error_code(err_code);
                    }
                    return seastar::make_ready_future<seastar::sstring>(response.SerializeAsString());
                });

            // TODO(pancake): time cost
        });
}

void DatanodeServer::RegisterServicePtr(void *service_ptr) {
    service_ptr_ = service_ptr;;
}

void DatanodeServer::RegisterAddExtentHandler(AddExtentHandler handler) {
    add_extent_handler_ = std::move(handler);

    rpc_.register_handler(DatanodeRpc::DATANODE_RPC_ADD_EXTENT,
        [this](seastar::sstring request_data) {
            AddExtentRequest request{};
            AddExtentResponse response{};
            request.ParseFromString(request_data.c_str());

            return add_extent_handler_(service_ptr_, request, &response)
                .then([this, &response](ErrorCode err_code) {
                    if (comm::IsNotOk(err_code)) {
                        logger_.error("add_extent failed. err:{}", comm::ErrorCodeName(err_code));
                        response.mutable_response_common()->set_error_code(err_code);
                    }
                    return seastar::make_ready_future<seastar::sstring>(response.SerializeAsString());
                });

            // TODO(pancake): time cost
        });
}

void DatanodeServer::RegisterDelExtentHandler(DelExtentHandler handler) {
    del_extent_handler_ = std::move(handler);

    rpc_.register_handler(DatanodeRpc::DATANODE_RPC_DEL_EXTENT,
        [this](seastar::sstring request_data) {
            DelExtentRequest request{};
            DelExtentResponse response{};
            request.ParseFromString(request_data.c_str());

            return del_extent_handler_(service_ptr_, request, &response)
                .then([this, &response](ErrorCode err_code) {
                    if (comm::IsNotOk(err_code)) {
                        logger_.error("del_extent failed. err:{}", comm::ErrorCodeName(err_code));
                        response.mutable_response_common()->set_error_code(err_code);
                    }
                    return seastar::make_ready_future<seastar::sstring>(response.SerializeAsString());
                });

            // TODO(pancake): time cost
        });
}

void DatanodeServer::RegisterSealExtentHandler(SealExtentHandler handler) {
    seal_extent_handler_ = std::move(handler);

    rpc_.register_handler(DatanodeRpc::DATANODE_RPC_SEAL_EXTENT,
        [this](seastar::sstring request_data) {
            SealExtentRequest request{};
            SealExtentResponse response{};
            request.ParseFromString(request_data.c_str());

            return seal_extent_handler_(service_ptr_, request, &response)
                .then([this, &response](ErrorCode err_code) {
                    if (comm::IsNotOk(err_code)) {
                        logger_.error("seal_extent failed. err:{}", comm::ErrorCodeName(err_code));
                        response.mutable_response_common()->set_error_code(err_code);
                    }
                    return seastar::make_ready_future<seastar::sstring>(response.SerializeAsString());
                });

            // TODO(pancake): time cost
        });
}

void DatanodeServer::RegisterListExtentsHandler(ListExtentHandler handler) {
    list_extents_handler_ = std::move(handler);

    rpc_.register_handler(DatanodeRpc::DATANODE_RPC_LIST_EXTENTS,
        [this](seastar::sstring request_data) {
            ListExtentsRequest request{};
            ListExtentsResponse response{};
            request.ParseFromString(request_data.c_str());

            return list_extents_handler_(service_ptr_, request, &response)
                .then([this, &response](ErrorCode err_code) {
                    if (comm::IsNotOk(err_code)) {
                        logger_.error("list_extents failed. err:{}", comm::ErrorCodeName(err_code));
                        response.mutable_response_common()->set_error_code(err_code);
                    }
                    return seastar::make_ready_future<seastar::sstring>(response.SerializeAsString());
                });

            // TODO(pancake): time cost
        });
}

void DatanodeServer::RegisterWriteAtHandler(WriteAtHandler handler) {
    write_at_handler_ = std::move(handler);

    rpc_.register_handler(DatanodeRpc::DATANODE_RPC_WRITE_AT,
        [this](seastar::sstring request_data) {
            WriteAtRequest request{};
            WriteAtResponse response{};
            request.ParseFromString(request_data.c_str());

            return write_at_handler_(service_ptr_, request, &response)
                .then([this, &response](ErrorCode err_code) {
                    if (comm::IsNotOk(err_code)) {
                        logger_.error("write_at_extent failed. err:{}", comm::ErrorCodeName(err_code));
                        response.mutable_response_common()->set_error_code(err_code);
                    }
                    return seastar::make_ready_future<seastar::sstring>(response.SerializeAsString());
                });

            // TODO(pancake): time cost
        });
}

void DatanodeServer::RegisterReadAtHandler(ReadAtHandler handler) {
    read_at_handler_ = std::move(handler);
    rpc_.register_handler(DatanodeRpc::DATANODE_RPC_READ_AT,
        [this](seastar::sstring request_data) {
            ReadAtRequest request{};
            ReadAtResponse response{};
            request.ParseFromString(request_data.c_str());

            return read_at_handler_(service_ptr_, request, &response)
                .then([this, &response](ErrorCode err_code) {
                    if (comm::IsNotOk(err_code)) {
                        logger_.error("read_at_extent failed. err:{}", comm::ErrorCodeName(err_code));
                        response.mutable_response_common()->set_error_code(err_code);
                    }
                    return seastar::make_ready_future<seastar::sstring>(response.SerializeAsString());
                });

            // TODO(pancake): time cost
        });
}

} // namespace pancake_store::rpc