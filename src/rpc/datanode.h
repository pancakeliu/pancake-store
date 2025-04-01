#pragma once

#include <seastar/rpc/rpc.hh>

#include "proto/datanode_rpc.pb.h"

#include "src/comm/non_copyable.h"
#include "src/comm/error_code.h"
#include "src/rpc/serializer.h"

namespace pancake_store::rpc {

using pancake_store::comm::NonCopyable;
using pancake_store::comm::ErrorCode;

using proto::service::datanode::AddDeviceRequest;
using proto::service::datanode::AddDeviceResponse;
using proto::service::datanode::DelDeviceRequest;
using proto::service::datanode::DelDeviceResponse;

using proto::service::datanode::AddExtentRequest;
using proto::service::datanode::AddExtentResponse;
using proto::service::datanode::DelExtentRequest;
using proto::service::datanode::DelExtentResponse;
using proto::service::datanode::SealExtentRequest;
using proto::service::datanode::SealExtentResponse;
using proto::service::datanode::ListExtentsRequest;
using proto::service::datanode::ListExtentsResponse;

using proto::service::datanode::WriteAtRequest;
using proto::service::datanode::WriteAtResponse;
using proto::service::datanode::ReadAtRequest;
using proto::service::datanode::ReadAtResponse;

// device manager rpc
using AddDeviceHandler = std::function<ErrorCode(const AddDeviceRequest&, AddDeviceResponse*)>;
using DelDeviceHandler = std::function<ErrorCode(const DelDeviceRequest&, DelDeviceResponse*)>;

class DatanodeDeviceServer : public NonCopyable {
public:
    DatanodeDeviceServer() {
        rpc_.set_logger(&logger_);
    }
    ~DatanodeDeviceServer() = default;

    GET_RPC_METHOD_INLINE

    void RegisterAddDeviceHandler(AddDeviceHandler handler);
    void RegisterDelDeviceHandler(DelDeviceHandler handler);

private:
    seastar::rpc::protocol<Serializer> rpc_{Serializer{}};
    seastar::logger logger_{"rpc.datanode_device_server"};

    AddDeviceHandler add_device_handler_;
    DelDeviceHandler del_device_handler_;
};

#define ADD_DEVICE_CALLER(rpc) ((rpc).make_client<seastar::sstring(seastar::sstring)>(DatanodeRpc::DATANODE_RPC_ADD_DEVICE))
#define DEL_DEVICE_CALLER(rpc) ((rpc).make_client<seastar::sstring(seastar::sstring)>(DatanodeRpc::DATANODE_RPC_DEL_DEVICE))

// Datanode Server
using AddExtentHandler = std::function<ErrorCode(const AddExtentRequest&, AddExtentResponse*)>;
using DelExtentHandler = std::function<ErrorCode(const DelExtentRequest&, DelExtentResponse*)>;
using SealExtentHandler = std::function<ErrorCode(const SealExtentRequest&, SealExtentResponse*)>;
using ListExtentHandler = std::function<ErrorCode(const ListExtentsRequest&, ListExtentsResponse*)>;
using WriteAtHandler = std::function<ErrorCode(const WriteAtRequest&, WriteAtResponse*)>;
using ReadAtHandler = std::function<ErrorCode(const ReadAtRequest&, ReadAtResponse*)>;

class DatanodeServer : public NonCopyable {
public:
    DatanodeServer() {
        rpc_.set_logger(&logger_);
    }
    ~DatanodeServer() = default;

    GET_RPC_METHOD_INLINE

    void RegisterAddExtentHandler(AddExtentHandler handler);
    void RegisterDelExtentHandler(DelExtentHandler handler);
    void RegisterSealExtentHandler(SealExtentHandler handler);
    void RegisterListExtentsHandler(ListExtentHandler handler);
    void RegisterWriteAtHandler(WriteAtHandler handler);
    void RegisterReadAtHandler(ReadAtHandler handler);

private:
    seastar::rpc::protocol<Serializer> rpc_{Serializer{}};
    seastar::logger logger_{"rpc.datanode_server"};

    AddExtentHandler add_extent_handler_;
    DelExtentHandler del_extent_handler_;
    SealExtentHandler seal_extent_handler_;
    ListExtentHandler list_extents_handler_;
    WriteAtHandler write_at_handler_;
    ReadAtHandler read_at_handler_;
};

#define ADD_EXTENT_CALLER(rpc) ((rpc).make_client<seastar::sstring(seastar::sstring)>(DatanodeRpc::DATANODE_RPC_ADD_EXTENT))
#define DEL_EXTENT_CALLER(rpc) ((rpc).make_client<seastar::sstring(seastar::sstring)>(DatanodeRpc::DATANODE_RPC_DEL_EXTENT))
#define SEAL_EXTENT_CALLER(rpc) ((rpc).make_client<seastar::sstring(seastar::sstring)>(DatanodeRpc::DATANODE_RPC_SEAL_EXTENT))
#define LIST_EXTENTS_CALLER(rpc) ((rpc).make_client<seastar::sstring(seastar::sstring)>(DatanodeRpc::DATANODE_RPC_LIST_EXTENTS))
#define READ_AT_CALLER(rpc) ((rpc).make_client<seastar::sstring(seastar::sstring)>(DatanodeRpc::DATANODE_RPC_READ_AT))
#define WRITE_AT_CALLER(rpc) ((rpc).make_client<seastar::sstring(seastar::sstring)>(DatanodeRpc::DATANODE_RPC_WRITE_AT))

} // namespace pancake_store::rpc

