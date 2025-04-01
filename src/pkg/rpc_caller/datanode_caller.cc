
#include "src/pkg/rpc_caller/datanode_caller.h"

namespace pancake_store::pkg::rpc_caller {

DatanodeCaller::DatanodeCaller(const seastar::sstring &ip, const uint16_t port) {
    client_ = std::make_shared<seastar::rpc::protocol<Serializer>::client>(rpc_, seastar::ipv4_addr{ip, port});
}

DatanodeCaller::~DatanodeCaller(){
    if (!closed_) {
        logger_.error("code error!!, datanode caller not close");
        // TODO(pancake): de-terminate???
        std::terminate();
    }
}

seastar::future<> DatanodeCaller::Close() {
    return client_->stop()
        .then([this] {
            closed_ = true;
            return seastar::make_ready_future<>();
        });
}

seastar::future<ErrorCode> DatanodeCaller::AddExtent(const AddExtentRequest &request, AddExtentResponse *response) {
    auto caller = ADD_EXTENT_CALLER(rpc_);

    return caller(*client_, request.SerializeAsString())
    .then([this, response](const seastar::sstring& resp_str) {
        auto ret = response->ParseFromString(resp_str);
        if (ret != 0) {
            logger_.error("Failed to parse add_extent response. ret:{}", ret);
            return seastar::make_ready_future<ErrorCode>(ErrorCode::RPC_CALLER_RESPONSE_PARSE_FAILED);
        }

        return seastar::make_ready_future<ErrorCode>(response->response_common().error_code());
    })
    .handle_exception([this](auto ep) {
         logger_.error("add_extent handle exception:{}", ep);
         return seastar::make_ready_future<ErrorCode>(ErrorCode::RPC_CALLER_INVOKE_FAILED);
    });
}

seastar::future<ErrorCode> DatanodeCaller::DelExtent(const DelExtentRequest &request, DelExtentResponse *response) {
    auto caller = DEL_EXTENT_CALLER(rpc_);

    return caller(*client_, request.SerializeAsString())
    .then([this, response](const seastar::sstring& resp_str) {
        auto ret = response->ParseFromString(resp_str);
        if (ret != 0) {
            logger_.error("Failed to parse del_extent response. ret:{}", ret);
            return seastar::make_ready_future<ErrorCode>(ErrorCode::RPC_CALLER_RESPONSE_PARSE_FAILED);
        }

        return seastar::make_ready_future<ErrorCode>(response->response_common().error_code());
    })
    .handle_exception([this](auto ep) {
         logger_.error("del_extent handle exception:{}", ep);
         return seastar::make_ready_future<ErrorCode>(ErrorCode::RPC_CALLER_INVOKE_FAILED);
    });
}

seastar::future<ErrorCode> DatanodeCaller::SealExtent(const SealExtentRequest &request, SealExtentResponse *response) {
    auto caller = SEAL_EXTENT_CALLER(rpc_);

    return caller(*client_, request.SerializeAsString())
    .then([this, response](const seastar::sstring& resp_str) {
        auto ret = response->ParseFromString(resp_str);
        if (ret != 0) {
            logger_.error("Failed to parse seal_extent response. ret:{}", ret);
            return seastar::make_ready_future<ErrorCode>(ErrorCode::RPC_CALLER_RESPONSE_PARSE_FAILED);
        }

        return seastar::make_ready_future<ErrorCode>(response->response_common().error_code());
    })
    .handle_exception([this](auto ep) {
         logger_.error("seal_extent handle exception:{}", ep);
         return seastar::make_ready_future<ErrorCode>(ErrorCode::RPC_CALLER_INVOKE_FAILED);
    });
}

seastar::future<ErrorCode> DatanodeCaller::ListExtents(const ListExtentsRequest &request, ListExtentsResponse *response) {
    auto caller = LIST_EXTENTS_CALLER(rpc_);

    return caller(*client_, request.SerializeAsString())
    .then([this, response](const seastar::sstring& resp_str) {
        auto ret = response->ParseFromString(resp_str);
        if (ret != 0) {
            logger_.error("Failed to parse list_extents response. ret:{}", ret);
            return seastar::make_ready_future<ErrorCode>(ErrorCode::RPC_CALLER_RESPONSE_PARSE_FAILED);
        }

        return seastar::make_ready_future<ErrorCode>(response->response_common().error_code());
    })
    .handle_exception([this](auto ep) {
         logger_.error("list_extents handle exception:{}", ep);
         return seastar::make_ready_future<ErrorCode>(ErrorCode::RPC_CALLER_INVOKE_FAILED);
    });
}

seastar::future<ErrorCode> DatanodeCaller::ReadAt(const ReadAtRequest &request, ReadAtResponse *response) {
    auto caller = READ_AT_CALLER(rpc_);

    return caller(*client_, request.SerializeAsString())
    .then([this, response](const seastar::sstring& resp_str) {
        auto ret = response->ParseFromString(resp_str);
        if (ret != 0) {
            logger_.error("Failed to parse read_at response. ret:{}", ret);
            return seastar::make_ready_future<ErrorCode>(ErrorCode::RPC_CALLER_RESPONSE_PARSE_FAILED);
        }

        return seastar::make_ready_future<ErrorCode>(response->response_common().error_code());
    })
    .handle_exception([this](auto ep) {
         logger_.error("read_at handle exception:{}", ep);
         return seastar::make_ready_future<ErrorCode>(ErrorCode::RPC_CALLER_INVOKE_FAILED);
    });
}

seastar::future<ErrorCode> DatanodeCaller::WriteAt(const WriteAtRequest &request, WriteAtResponse *response) {
    auto caller = WRITE_AT_CALLER(rpc_);

    return caller(*client_, request.SerializeAsString())
    .then([this, response, request](const seastar::sstring& resp_str) {
        auto ret = response->ParseFromString(resp_str);
        if (ret != 0) {
            logger_.error("Failed to parse write_at response. ret:{}, request:{}",
                ret, request.ShortDebugString());
            return seastar::make_ready_future<ErrorCode>(ErrorCode::RPC_CALLER_RESPONSE_PARSE_FAILED);
        }

        return seastar::make_ready_future<ErrorCode>(response->response_common().error_code());
    })
    .handle_exception([this, request](auto ep) {
         logger_.error("write_at handle exception:{}, request:{}", ep, request.ShortDebugString());
         return seastar::make_ready_future<ErrorCode>(ErrorCode::RPC_CALLER_INVOKE_FAILED);
    });
}

} // namespace pancake_store::pkg::rpc_caller