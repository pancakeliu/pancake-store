# datanode storage module cmake

# storage engine directory
set(DATANODE_STORAGE_ENGINE_DIR ${DATANODE_STORAGE_DIR}/engine)
add_subdirectory(${DATANODE_STORAGE_ENGINE_DIR})

file(
        GLOB
        DATANODE_STORAGE_SOURCES_AND_HEADERS
        "${DATANODE_STORAGE_DIR}/*.h"
        "${DATANODE_STORAGE_DIR}/*.cc"
)

include_directories(
        ${SEASTAR_INCLUDE}
)

find_package (Seastar REQUIRED)

add_library(pc-datanode-storage OBJECT ${DATANODE_SERVICE_SOURCES_AND_HEADERS})
target_link_libraries(
        pc-datanode-storage
        PUBLIC
        Seastar::seastar
)