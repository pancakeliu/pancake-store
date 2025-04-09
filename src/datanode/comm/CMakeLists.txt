# datanode common module cmake

file(
        GLOB_RECURSE
        DATANODE_COMM_SOURCES_AND_HEADERS
        "${DATANODE_COMM_DIR}/*.h"
        "${DATANODE_COMM_DIR}/*.cc"
)

include_directories(
        ${SEASTAR_INCLUDE}
)

add_library(pc-datanode-comm OBJECT ${DATANODE_COMM_SOURCES_AND_HEADERS})
target_link_libraries(
        pc-datanode-comm
        PUBLIC
        Seastar::seastar
        yaml-cpp::yaml-cpp
)