# pancake-store client tool cmake

add_executable(pc-client ${PANCAKE_STORE_TOOLS_CLIENT_DIR}/main.cc)
add_dependencies(pc-client  DepsFinished)

set(CMAKE_CXX_COMPILER "/usr/bin/g++")

target_compile_options(pc-client BEFORE PRIVATE
        -std=c++20
        -fcoroutines
        -Wno-c++23-extensions  # 如果有 C++23 的警告
)

find_package(Seastar REQUIRED)

target_link_libraries(
        pc-client
        PUBLIC
        pc-datanode-service
        pc-rpc
        pc-proto
        Seastar::seastar
        protobuf
)