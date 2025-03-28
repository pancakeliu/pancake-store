#pragma once

#include <seastar/rpc/rpc.hh>

#include "proto/c2s_datanode.pb.h"

#include "src/comm/non_copyable.h"
#include "src/comm/error_code.h"
#include "src/rpc/serializer.h"

namespace pancake_store::rpc {

using pancake_store::comm::NonCopyable;
using pancake_store::comm::ErrorCode;

using proto::service::datanode::WriteAtRequest;
using proto::service::datanode::WriteAtResponse;
using proto::service::datanode::ReadAtRequest;
using proto::service::datanode::ReadAtResponse;

enum class C2SDatanodeAPI : unsigned {
    WriteAt = 1,
    ReadAt = 2,
};

using WriteAtHandler = std::function<ErrorCode(const WriteAtRequest&, WriteAtResponse*)>;
using ReadAtHandler = std::function<ErrorCode(const ReadAtRequest&, ReadAtResponse*)>;

class C2SDatanodeServer : public NonCopyable {
public:
    C2SDatanodeServer() {
        rpc_.set_logger(&logger_);
    }
    ~C2SDatanodeServer() = default;

    GET_RPC_METHOD_INLINE

    void RegisterWriteAtHandler(WriteAtHandler handler);
    void RegisterReadAtHandler(ReadAtHandler handler);

private:
    seastar::rpc::protocol<Serializer> rpc_{Serializer{}};
    seastar::logger logger_{"rpc_c2s_datanode_server"};

    WriteAtHandler write_at_handler_;
    ReadAtHandler read_at_handler_;
};

class C2SDatanodeClient : public NonCopyable {
public:
    C2SDatanodeClient() {
        rpc_.set_logger(&logger_);
    }
    ~C2SDatanodeClient() = default;

    GET_RPC_METHOD_INLINE

    auto WriteAtCaller();
    auto ReadAtCaller();

private:
    seastar::rpc::protocol<Serializer> rpc_{Serializer{}};
    seastar::logger logger_{"rpc_c2s_datanode_client"};
};

} // namespace pancake_store::rpc

