
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

using proto::service::datanode::AddExtentRequest;
using proto::service::datanode::AddExtentResponse;
using proto::service::datanode::DelExtentRequest;
using proto::service::datanode::DelExtentResponse;
using proto::service::datanode::SealExtentRequest;
using proto::service::datanode::SealExtentResponse;
using proto::service::datanode::ListExtentsRequest;
using proto::service::datanode::ListExtentsResponse;
using proto::service::datanode::ReadAtRequest;
using proto::service::datanode::ReadAtResponse;
using proto::service::datanode::WriteAtRequest;
using proto::service::datanode::WriteAtResponse;

class DatanodeCaller : public NonCopyable {
public:
    DatanodeCaller(const seastar::sstring &ip, uint16_t port);
    ~DatanodeCaller();

    seastar::future<> Close();

    seastar::future<ErrorCode> AddExtent(const AddExtentRequest &request, AddExtentResponse *response);
    seastar::future<ErrorCode> DelExtent(const DelExtentRequest &request, DelExtentResponse *response);
    seastar::future<ErrorCode> SealExtent(const SealExtentRequest &request, SealExtentResponse *response);
    seastar::future<ErrorCode> ListExtents(const ListExtentsRequest &request, ListExtentsResponse *response);
    seastar::future<ErrorCode> ReadAt(const ReadAtRequest &request, ReadAtResponse *response);
    seastar::future<ErrorCode> WriteAt(const WriteAtRequest &request, WriteAtResponse *response);

private:
    std::shared_ptr<seastar::rpc::protocol<Serializer>::client> client_;
    seastar::rpc::protocol<Serializer> rpc_{Serializer{}};
    seastar::logger logger_{"datanode_caller"};

    bool closed_{false};
};

} // namespace pancake_store::pkg::rpc_caller