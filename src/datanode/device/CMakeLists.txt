# datanode device cmake

file(
        GLOB_RECURSE
        DATANODE_DEVICE_SOURCES_AND_HEADERS
        "${DATANODE_DEVICE_DIR}/*.h"
        "${DATANODE_DEVICE_DIR}/*.cc"
)

include_directories(
        ${SEASTAR_INCLUDE}
)

find_package (Seastar REQUIRED)

add_library(pc-datanode-device OBJECT ${DATANODE_DEVICE_SOURCES_AND_HEADERS})
target_link_libraries(
        pc-datanode-device
        PUBLIC
        Seastar::seastar
)