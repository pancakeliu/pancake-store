
#include <seastar/core/app-template.hh>
#include <seastar/core/reactor.hh>
#include <seastar/core/sleep.hh>

#include "src/comm/error_code.h"
#include "src/datanode/service/device_service.h"

#include <cassert>

using pancake_store::comm::ErrorCode;
using pancake_store::comm::ErrorCodeName;

using pancake_store::datanode::service::DeviceService;

constexpr unsigned main_worker_cpu_id = 0;

int main(const int argc, char **argv) {
    seastar::app_template app;
    seastar::logger logger("datanode.main");

    app.add_options()
        ("version,v", boost::program_options::value<bool>()->default_value(false), "pancake store version")
        ("device-server-port", boost::program_options::value<uint16_t>()->default_value(8888), "datanode device server listen port")
        ("http-port", boost::program_options::value<uint16_t>()->default_value(9999), "datanode http server listen port");

    uint16_t device_server_port = 8888;

    return app.run(argc, argv, [&logger, device_server_port] {
        // start first worker, run in 0 cpu
        return seastar::smp::submit_to(main_worker_cpu_id, [&logger, device_server_port] {
            // check shard id
            assert(seastar::this_shard_id() == main_worker_cpu_id);
            logger.info("start datanode main worker. current_cpu:{}", seastar::this_shard_id());

            // start rpc
            (void)DeviceService::Instance().Start(device_server_port).then([&logger]() {
                logger.info("device service start success!!");
                return seastar::make_ready_future<>();
            });

            return seastar::keep_doing([&logger] {
                logger.info("okok....");
                return seastar::sleep(std::chrono::milliseconds(100));
            });
        });
    });
}