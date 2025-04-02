
#include "src/rpc/datanode.h"

#include <utility>

#include <seastar/util/defer.hh>

namespace pancake_store::rpc {

using proto::service::datanode::DatanodeRpc;

void DatanodeDeviceServer::RegisterServicePtr(void *service_ptr) {
    service_ptr_ = service_ptr;
}

void DatanodeDeviceServer::RegisterAddDeviceHandler(AddDeviceHandler handler) {
    add_device_handler_ = std::move(handler);

    rpc_.register_handler(DatanodeRpc::DATANODE_RPC_ADD_DEVICE,
        [this](seastar::sstring request_data) {
            auto *request = new AddDeviceRequest;
            auto *response = new AddDeviceResponse;
            request->ParseFromString(request_data.c_str());

            auto finish_cb = [request, response, this] {
                delete request;
                delete response;
            };

            return add_device_handler_(service_ptr_, *request, response)
                .then([this, response, cb = std::move(finish_cb)](ErrorCode err_code) {
                    auto defer = seastar::defer(cb);

                    if (comm::IsNotOk(err_code)) {
                        logger_.error("add_device failed. err:{}", comm::ErrorCodeName(err_code));
                        response->mutable_response_common()->set_error_code(err_code);
                    }

                    return seastar::make_ready_future<seastar::sstring>(response->SerializeAsString());
                });
        });
}

void DatanodeDeviceServer::RegisterDelDeviceHandler(DelDeviceHandler handler) {
    del_device_handler_ = std::move(handler);

    rpc_.register_handler(DatanodeRpc::DATANODE_RPC_ADD_DEVICE,
        [this](seastar::sstring request_data) {
            auto *request = new DelDeviceRequest;
            auto *response = new DelDeviceResponse;
            request->ParseFromString(request_data.c_str());

            auto finish_cb = [request, response, this] {
                delete request;
                delete response;
            };

            return del_device_handler_(service_ptr_, *request, response)
                .then([this, response, cb = std::move(finish_cb)](ErrorCode err_code) {
                    auto defer = seastar::defer(cb);

                    if (comm::IsNotOk(err_code)) {
                        logger_.error("del_device failed. err:{}", comm::ErrorCodeName(err_code));
                        response->mutable_response_common()->set_error_code(err_code);
                    }

                    return seastar::make_ready_future<seastar::sstring>(response->SerializeAsString());
                });
        });
}

void DatanodeServer::RegisterServicePtr(void *service_ptr) {
    service_ptr_ = service_ptr;;
}

void DatanodeServer::RegisterAddExtentHandler(AddExtentHandler handler) {
    add_extent_handler_ = std::move(handler);

    rpc_.register_handler(DatanodeRpc::DATANODE_RPC_ADD_DEVICE,
        [this](seastar::sstring request_data) {
            auto *request = new AddExtentRequest;
            auto *response = new AddExtentResponse;
            request->ParseFromString(request_data.c_str());

            auto finish_cb = [request, response, this] {
                delete request;
                delete response;
            };

            return add_extent_handler_(service_ptr_, *request, response)
                .then([this, response, cb = std::move(finish_cb)](ErrorCode err_code) {
                    auto defer = seastar::defer(cb);

                    if (comm::IsNotOk(err_code)) {
                        logger_.error("add_extent failed. err:{}", comm::ErrorCodeName(err_code));
                        response->mutable_response_common()->set_error_code(err_code);
                    }

                    return seastar::make_ready_future<seastar::sstring>(response->SerializeAsString());
                });
        });
}

void DatanodeServer::RegisterDelExtentHandler(DelExtentHandler handler) {
    del_extent_handler_ = std::move(handler);

    rpc_.register_handler(DatanodeRpc::DATANODE_RPC_ADD_DEVICE,
        [this](seastar::sstring request_data) {
            auto *request = new DelExtentRequest;
            auto *response = new DelExtentResponse;
            request->ParseFromString(request_data.c_str());

            auto finish_cb = [request, response, this] {
                delete request;
                delete response;
            };

            return del_extent_handler_(service_ptr_, *request, response)
                .then([this, response, cb = std::move(finish_cb)](ErrorCode err_code) {
                    auto defer = seastar::defer(cb);

                    if (comm::IsNotOk(err_code)) {
                        logger_.error("del_extent failed. err:{}", comm::ErrorCodeName(err_code));
                        response->mutable_response_common()->set_error_code(err_code);
                    }

                    return seastar::make_ready_future<seastar::sstring>(response->SerializeAsString());
                });
        });
}

void DatanodeServer::RegisterSealExtentHandler(SealExtentHandler handler) {
    seal_extent_handler_ = std::move(handler);

    rpc_.register_handler(DatanodeRpc::DATANODE_RPC_ADD_DEVICE,
        [this](seastar::sstring request_data) {
            auto *request = new SealExtentRequest;
            auto *response = new SealExtentResponse;
            request->ParseFromString(request_data.c_str());

            auto finish_cb = [request, response, this] {
                delete request;
                delete response;
            };

            return seal_extent_handler_(service_ptr_, *request, response)
                .then([this, response, cb = std::move(finish_cb)](ErrorCode err_code) {
                    auto defer = seastar::defer(cb);

                    if (comm::IsNotOk(err_code)) {
                        logger_.error("seal_extent failed. err:{}", comm::ErrorCodeName(err_code));
                        response->mutable_response_common()->set_error_code(err_code);
                    }

                    return seastar::make_ready_future<seastar::sstring>(response->SerializeAsString());
                });
        });
}

void DatanodeServer::RegisterListExtentsHandler(ListExtentHandler handler) {
    list_extents_handler_ = std::move(handler);

    rpc_.register_handler(DatanodeRpc::DATANODE_RPC_ADD_DEVICE,
        [this](seastar::sstring request_data) {
            auto *request = new ListExtentsRequest;
            auto *response = new ListExtentsResponse;
            request->ParseFromString(request_data.c_str());

            auto finish_cb = [request, response, this] {
                delete request;
                delete response;
            };

            return list_extents_handler_(service_ptr_, *request, response)
                .then([this, response, cb = std::move(finish_cb)](ErrorCode err_code) {
                    auto defer = seastar::defer(cb);

                    if (comm::IsNotOk(err_code)) {
                        logger_.error("list_extents failed. err:{}", comm::ErrorCodeName(err_code));
                        response->mutable_response_common()->set_error_code(err_code);
                    }

                    return seastar::make_ready_future<seastar::sstring>(response->SerializeAsString());
                });
        });
}

void DatanodeServer::RegisterWriteAtHandler(WriteAtHandler handler) {
    write_at_handler_ = std::move(handler);

    rpc_.register_handler(DatanodeRpc::DATANODE_RPC_ADD_DEVICE,
        [this](seastar::sstring request_data) {
            auto *request = new WriteAtRequest;
            auto *response = new WriteAtResponse;
            request->ParseFromString(request_data.c_str());

            auto finish_cb = [request, response, this] {
                delete request;
                delete response;
            };

            return write_at_handler_(service_ptr_, *request, response)
                .then([this, response, cb = std::move(finish_cb)](ErrorCode err_code) {
                    auto defer = seastar::defer(cb);

                    if (comm::IsNotOk(err_code)) {
                        logger_.error("write_at failed. err:{}", comm::ErrorCodeName(err_code));
                        response->mutable_response_common()->set_error_code(err_code);
                    }

                    return seastar::make_ready_future<seastar::sstring>(response->SerializeAsString());
                });
        });
}

void DatanodeServer::RegisterReadAtHandler(ReadAtHandler handler) {
    read_at_handler_ = std::move(handler);

    rpc_.register_handler(DatanodeRpc::DATANODE_RPC_ADD_DEVICE,
        [this](seastar::sstring request_data) {
            auto *request = new ReadAtRequest;
            auto *response = new ReadAtResponse;
            request->ParseFromString(request_data.c_str());

            auto finish_cb = [request, response, this] {
                delete request;
                delete response;
            };

            return read_at_handler_(service_ptr_, *request, response)
                .then([this, response, cb = std::move(finish_cb)](ErrorCode err_code) {
                    auto defer = seastar::defer(cb);

                    if (comm::IsNotOk(err_code)) {
                        logger_.error("read_at failed. err:{}", comm::ErrorCodeName(err_code));
                        response->mutable_response_common()->set_error_code(err_code);
                    }

                    return seastar::make_ready_future<seastar::sstring>(response->SerializeAsString());
                });
        });
}

} // namespace pancake_store::rpc