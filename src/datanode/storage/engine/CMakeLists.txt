# datanode storage engine module cmake

# storage pancake engine directory
set(STORAGE_PANCAKE_ENGINE_DIR ${DATANODE_STORAGE_ENGINE_DIR}/pancake)
add_subdirectory(${STORAGE_PANCAKE_ENGINE_DIR})

file(
        GLOB
        DATANODE_STORAGE_ENGINE_SOURCES_AND_HEADERS
        "${DATANODE_STORAGE_ENGINE_DIR}/*.h"
        "${DATANODE_STORAGE_ENGINE_DIR}/*.cc"
)

include_directories(
        ${SEASTAR_INCLUDE}
)

add_library(pc-datanode-storage-engine OBJECT ${DATANODE_STORAGE_ENGINE_SOURCES_AND_HEADERS})
target_link_libraries(
        pc-datanode-storage-engine
        PUBLIC
        Seastar::seastar
)