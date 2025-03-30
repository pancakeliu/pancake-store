
#include <seastar/core/app-template.hh>
#include <seastar/core/reactor.hh>

#include "src/comm/error_code.h"

using pancake_store::comm::ErrorCode;
using pancake_store::comm::ErrorCodeName;

constexpr unsigned main_worker_cpu_id = 0;

int main(const int argc, char **argv) {
    seastar::app_template app;
    seastar::logger logger("datanode.main");

    app.add_options()
        ("version,v", boost::program_options::value<bool>()->default_value(false), "pancake store version")
        ("rpc-port", boost::program_options::value<uint16_t>()->default_value(8888), "datanode rpc server listen port")
        ("http-port", boost::program_options::value<uint16_t>()->default_value(9999), "datanode http server listen port");

    return app.run(argc, argv, [&logger] {
        // start first worker, run in 0 cpu
        return seastar::smp::submit_to(main_worker_cpu_id, [&logger] {
            logger.info("start datanode main worker");
            return seastar::make_ready_future<ErrorCode>(ErrorCode::PANCAKE_STORE_OK);
        }).then([&logger] (const ErrorCode error_code) {
            logger.error("datanode main worker run failed. err_code:%s", ErrorCodeName(error_code));
            return seastar::make_ready_future<>();
        });
    });
}