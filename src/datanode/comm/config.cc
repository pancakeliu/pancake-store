
#include "src/datanode/comm/config.h"
#include "src/comm/ip_address.h"

#include <yaml-cpp/yaml.h>

namespace pancake_store::datanode::comm {

using pancake_store::comm::IPAddress;

constexpr uint16_t k_system_min_listen_port = 4096;

ErrorCode Config::ParseConfig(const std::string &config_path) {
    if (config_path.empty()) {
        logger_.error("config file path empty.");
        return ErrorCode::PANCAKE_STORE_YAML_CONFIG_INVALID;
    }

    const YAML::Node config_node = YAML::LoadFile(config_path);

    if (!config_node.IsDefined() || !config_node.IsMap()) {
        logger_.error("yaml config not valid. conf_file:{}", config_path);
        return ErrorCode::PANCAKE_STORE_YAML_CONFIG_INVALID;
    }

    // yaml config check
    if (!config_node["datanode_id"] || config_node["datanode_id"].IsScalar()) {
        logger_.error("datanode_id config not found in yaml conf_file:{}", config_path);
        return ErrorCode::PANCAKE_STORE_YAML_CONFIG_INVALID;
    }
    if (!config_node["datanode_ip"] || config_node["datanode_ip"].IsScalar()) {
        logger_.error("datanode_ip config not found in yaml conf_file:{}", config_path);
        return ErrorCode::PANCAKE_STORE_YAML_CONFIG_INVALID;
    }
    if (!config_node["cluster_id"] || config_node["cluster_id"].IsScalar()) {
        logger_.error("cluster_id config not found in yaml conf_file:{}", config_path);
        return ErrorCode::PANCAKE_STORE_YAML_CONFIG_INVALID;
    }

    if (!config_node["http_server_port"] || config_node["http_server_port"].IsScalar()) {
        logger_.error("http_server_port config not found in yaml conf_file:{}", config_path);
        return ErrorCode::PANCAKE_STORE_YAML_CONFIG_INVALID;
    }
    if (!config_node["manager_service_port"] || config_node["manager_service_port"].IsScalar()) {
        logger_.error("manager_service_port config not found in yaml conf_file:{}", config_path);
        return ErrorCode::PANCAKE_STORE_YAML_CONFIG_INVALID;
    }
    if (!config_node["data_service_port_min"] || config_node["data_service_port_min"].IsScalar()) {
        logger_.error("data_service_port_min config not found in yaml conf_file:{}", config_path);
        return ErrorCode::PANCAKE_STORE_YAML_CONFIG_INVALID;
    }
    if (!config_node["data_service_port_max"] || config_node["data_service_port_max"].IsScalar()) {
        logger_.error("data_service_port_max config not found in yaml conf_file:{}", config_path);
        return ErrorCode::PANCAKE_STORE_YAML_CONFIG_INVALID;
    }

    datanode_id_ = config_node["datanode_id"].as<std::string>();
    datanode_ip_ = config_node["datanode_ip"].as<std::string>();
    cluster_id_ = config_node["cluster_id"].as<std::string>();

    http_server_port_ = config_node["http_server_port"].as<uint16_t>();
    manager_service_port_ = config_node["manager_service_port"].as<uint16_t>();
    data_service_port_min_ = config_node["data_service_port_min"].as<uint16_t>();
    data_service_port_max_ = config_node["data_service_port_max"].as<uint16_t>();

    return ErrorCode::PANCAKE_STORE_OK;
}

ErrorCode Config::CheckConfig() {
    if (datanode_id_.empty()) {
        logger_.error("datanode_id is empty");
        return ErrorCode::PANCAKE_STORE_YAML_CONFIG_INVALID;
    }
    if (!IPAddress::IsIPv4Address(datanode_ip_)) {
        logger_.error("datanode_ip:{} is invalid.", datanode_ip_);
        return ErrorCode::PANCAKE_STORE_YAML_CONFIG_INVALID;
    }
    if (cluster_id_.empty()) {
        logger_.error("cluster_id is empty");
        return ErrorCode::PANCAKE_STORE_YAML_CONFIG_INVALID;
    }
    if (http_server_port_ < k_system_min_listen_port) {
        logger_.error("http_server_port:{} is invalid.", http_server_port_);
        return ErrorCode::PANCAKE_STORE_YAML_CONFIG_INVALID;
    }
    if (manager_service_port_ < k_system_min_listen_port) {
        logger_.error("manager_service_port:{} is invalid.", manager_service_port_);
        return ErrorCode::PANCAKE_STORE_YAML_CONFIG_INVALID;
    }
    if (data_service_port_min_ < k_system_min_listen_port) {
        logger_.error("data_service_port_min:{} is invalid.", data_service_port_min_);
        return ErrorCode::PANCAKE_STORE_YAML_CONFIG_INVALID;
    }
    if (data_service_port_max_ < k_system_min_listen_port) {
        logger_.error("data_service_port_max:{} is invalid.", data_service_port_max_);
        return ErrorCode::PANCAKE_STORE_YAML_CONFIG_INVALID;
    }
    if (data_service_port_min_ > data_service_port_max_) {
        logger_.error("data_service_port config is invalid. min:{}, max:{}",
            data_service_port_min_, data_service_port_max_);
        return ErrorCode::PANCAKE_STORE_YAML_CONFIG_INVALID;
    }

    return ErrorCode::PANCAKE_STORE_OK;
}

} // namespace pancake_store::datanode::comm