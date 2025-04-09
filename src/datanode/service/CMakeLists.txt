# datanode service cmake

file(
        GLOB_RECURSE
        DATANODE_SERVICE_SOURCES_AND_HEADERS
        "${DATANODE_SERVICE_DIR}/*.h"
        "${DATANODE_SERVICE_DIR}/*.cc"
)

include_directories(
        ${SEASTAR_INCLUDE}
)

add_library(pc-datanode-service OBJECT ${DATANODE_SERVICE_SOURCES_AND_HEADERS})
target_link_libraries(
        pc-datanode-service
        PUBLIC
        Seastar::seastar
)