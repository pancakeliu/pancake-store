
#include <seastar/core/app-template.hh>
#include <seastar/core/sleep.hh>

#include "src/rpc/datanode.h"
#include "src/rpc/serializer.h"

using pancake_store::rpc::Serializer;

using proto::service::datanode::AddDeviceRequest;
using proto::service::datanode::AddDeviceResponse;
using proto::service::datanode::DatanodeRpc;

int main(int argc, char** argv) {
    seastar::app_template app;

    return app.run(argc, argv, [] {
        seastar::logger logger("tool.client");

        seastar::rpc::protocol<Serializer> rpc{Serializer{}};
        auto caller = ADD_DEVICE_CALLER(rpc);
        auto client = seastar::rpc::protocol<Serializer>::client(rpc, seastar::socket_address(seastar::ipv4_addr{"127.0.0.1", 8888}));

        // wait a moment
        return seastar::sleep(std::chrono::seconds(1)).then([&logger, &client, &caller] {
            AddDeviceRequest request;
            request.set_datanode_id("abc");
            request.set_disk_id(123);

            logger.info("start to send rpc:{}", request.ShortDebugString());

            return caller(client, request.SerializeAsString())
                .then([&logger](seastar::sstring resp_str) {
                    logger.info("got response");
                    AddDeviceResponse response;
                    auto ret = response.ParseFromString(resp_str);
                    if (ret != 0) {
                        logger.error("Failed to parse response. ret:{}", ret);
                        return seastar::make_ready_future<>();
                    }
                    logger.info("response:{}", response.ShortDebugString());

                    return seastar::make_ready_future<>();
                })
                .handle_exception([&logger](auto ep) {
                    logger.error("handle exception:{}", ep);
                    return seastar::make_ready_future<>();
                });
        });
    });
}