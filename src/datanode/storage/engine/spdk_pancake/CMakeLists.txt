# datanode storage spdk pancake engine module cmake

file(
        GLOB_RECURSE
        STORAGE_SPDK_PANCAKE_ENGINE_DIR_SOURCES_AND_HEADERS
        "${STORAGE_SPDK_PANCAKE_ENGINE_DIR}/*.h"
        "${STORAGE_SPDK_PANCAKE_ENGINE_DIR}/*.cc"
)

include_directories(
        ${SEASTAR_INCLUDE}
)

find_package (Seastar REQUIRED)

add_library(pc-spdk-datanode-pancake-engine OBJECT ${STORAGE_SPDK_PANCAKE_ENGINE_DIR_SOURCES_AND_HEADERS})
target_link_libraries(
        pc-datanode-spdk-pancake-engine
        PUBLIC
        Seastar::seastar
)