
#pragma once

#include "src/comm/error_code.h"
#include "src/comm/non_copyable.h"

#include "src/rpc/datanode.h"

#include <seastar/rpc/rpc.hh>

namespace pancake_store::pkg::rpc_caller {

using pancake_store::comm::ErrorCode;
using pancake_store::comm::NonCopyable;

using pancake_store::rpc::Serializer;

using proto::service::datanode::DatanodeRpc;

using proto::service::datanode::AddDeviceRequest;
using proto::service::datanode::AddDeviceResponse;
using proto::service::datanode::DelDeviceRequest;
using proto::service::datanode::DelDeviceResponse;

class DatanodeDeviceCaller : public NonCopyable {
public:
    DatanodeDeviceCaller(const seastar::sstring &ip, uint16_t port);
    ~DatanodeDeviceCaller();

    seastar::future<> Close();

    seastar::future<ErrorCode> AddDevice(const AddDeviceRequest &request, AddDeviceResponse *response);
    seastar::future<ErrorCode> DelDevice(const DelDeviceRequest &request, DelDeviceResponse *response);

private:
    std::shared_ptr<seastar::rpc::protocol<Serializer>::client> client_;
    seastar::rpc::protocol<Serializer> rpc_{Serializer{}};
    seastar::logger logger_{"datanode_device_caller"};

    bool closed_{false};
};

} // namespace pancake_store::pkg::rpc_caller