
#include <seastar/core/app-template.hh>
#include <seastar/core/reactor.hh>
#include <seastar/core/sleep.hh>

#include "src/comm/error_code.h"
#include "src/datanode/comm/config.h"
#include "src/datanode/service/manager_service.h"

#include <cassert>

using pancake_store::comm::ErrorCode;
using pancake_store::comm::ErrorCodeName;
using pancake_store::comm::IsNotOk;

using pancake_store::datanode::comm::Config;
using pancake_store::datanode::service::ManagerService;

constexpr unsigned main_worker_cpu_id = 0;

int main(const int argc, char **argv) {
    seastar::app_template app;
    seastar::logger logger("datanode.main");

    app.add_options()
           ("version,v", boost::program_options::value<bool>()->default_value(false), "pancake store version")
           ("config,c", boost::program_options::value<std::string>()->default_value(""), "pancake-store datanode config file");

    return app.run(argc, argv, [&] {
        const auto &config = app.configuration();
        if (config["version"].as<bool>()) {
            // TODO(pancake): print datanode version.
            return seastar::make_ready_future<>();
        }
        const auto config_file = config["config"].as<std::string>();
        auto ret = Config::Instance().ParseConfig(config_file);
        if (IsNotOk(ret)) {
            logger.error("config file parse error. err:{}, config_file:{}",
                ErrorCodeName(ret), config_file);
            return seastar::make_ready_future<>();
        }
        ret = Config::Instance().CheckConfig();
        if (IsNotOk(ret)) {
            logger.error("config file check error. err:{}, config_file:{}",
                ErrorCodeName(ret), config_file);
            return seastar::make_ready_future<>();
        }

        // start first worker, run in 0 cpu
        return seastar::smp::submit_to(main_worker_cpu_id, [&logger] {
            // check shard id
            assert(seastar::this_shard_id() == main_worker_cpu_id);
            logger.info("start datanode main worker. current_cpu:{}", seastar::this_shard_id());

            // start rpc
            (void)ManagerService::Instance().Start(Config::Instance().ManagerServerPort())
                .then([&logger]() {
                    logger.info("device service start success!!");
                    return seastar::make_ready_future<>();
                });

            // TODO(pancake): master heartbeat worker

            return seastar::keep_doing([&logger] {
                return seastar::sleep(std::chrono::seconds(60));
            });
        });
    });
}