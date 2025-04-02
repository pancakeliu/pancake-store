#pragma once

#include "src/comm/non_copyable.h"
#include "src/comm/error_code.h"

#include <seastar/util/log.hh>

namespace pancake_store::datanode::comm {

using pancake_store::comm::NonCopyable;
using pancake_store::comm::ErrorCode;

class Config : public NonCopyable {
public:
    static Config &Instance() {
        static Config instance;
        return instance;
    }

    ErrorCode ParseConfig(const std::string &config_path);
    ErrorCode CheckConfig();

    [[nodiscard]] std::string DatanodeId() const {
        return datanode_id_;
    }
    [[nodiscard]] std::string DatanodeIp() const {
        return datanode_ip_;
    }
    [[nodiscard]] std::string ClusterId() const {
        return cluster_id_;
    }

    [[nodiscard]] uint16_t HttpServerPort() const {
        return http_server_port_;
    }
    [[nodiscard]] uint16_t ManagerServerPort() const {
        return manager_service_port_;
    }
    [[nodiscard]] uint16_t DataServicePortMin() const {
        return data_service_port_min_;
    }
    [[nodiscard]] uint16_t DataServicePortMax() const {
        return data_service_port_max_;
    }

private:
    Config() = default;
    ~Config() = default;

private:
    std::string datanode_id_{};
    std::string datanode_ip_{};
    std::string cluster_id_{};

    uint16_t http_server_port_{};
    uint16_t manager_service_port_{};
    uint16_t data_service_port_min_{};
    uint16_t data_service_port_max_{};

    seastar::logger logger_{"datanode.config"};
};

} // namespace pancake_store::datanode::comm