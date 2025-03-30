
#include "src/rpc/datanode.h"

namespace pancake_store::rpc {

void C2SDatanodeServer::RegisterWriteAtHandler(WriteAtHandler handler) {
    write_at_handler_ = std::move(handler);

    rpc_.register_handler(static_cast<unsigned>(C2SDatanodeAPI::WriteAt),
        [this](const seastar::sstring &write_request_data) {
            WriteAtRequest request{};
            WriteAtResponse response{};
            request.ParseFromString(write_request_data);

            auto err_code = write_at_handler_(request, &response);
            if (comm::IsNotOk(err_code)) {
                logger_.error("write_at failed. err:%s", comm::ErrorCode_Name(err_code));
                response.set_error_code(err_code);
            }

            // TODO(pancake): time cost

            return seastar::make_ready_future<seastar::sstring>(response.SerializeAsString());
        });
}

void C2SDatanodeServer::RegisterReadAtHandler(ReadAtHandler handler) {
    read_at_handler_ = std::move(handler);
    rpc_.register_handler(static_cast<unsigned>(C2SDatanodeAPI::ReadAt),
        [this](const seastar::sstring &read_request_data) {
            ReadAtRequest request{};
            ReadAtResponse response{};
            request.ParseFromString(read_request_data);

            auto err_code = read_at_handler_(request, &response);
            if (comm::IsNotOk(err_code)) {
                logger_.error("read_at failed. err:%s", comm::ErrorCode_Name(err_code));
                response.set_error_code(err_code);
            }

            // TODO(pancake): time cost

            return seastar::make_ready_future<seastar::sstring>(response.SerializeAsString());
        });
}

auto C2SDatanodeClient::WriteAtCaller() {
    return rpc_.make_client<seastar::sstring(seastar::sstring)>(static_cast<unsigned>(C2SDatanodeAPI::WriteAt));
}

auto C2SDatanodeClient::ReadAtCaller() {
    return rpc_.make_client<seastar::sstring(seastar::sstring)>(static_cast<unsigned>(C2SDatanodeAPI::ReadAt));
}

} // namespace pancake_store::rpc