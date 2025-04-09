# datanode storage pancake engine module cmake

file(
        GLOB_RECURSE
        STORAGE_PANCAKE_ENGINE_DIR_SOURCES_AND_HEADERS
        "${STORAGE_PANCAKE_ENGINE_DIR}/*.h"
        "${STORAGE_PANCAKE_ENGINE_DIR}/*.cc"
)

include_directories(
        ${SEASTAR_INCLUDE}
)

add_library(pc-datanode-pancake-engine OBJECT ${STORAGE_PANCAKE_ENGINE_DIR_SOURCES_AND_HEADERS})
target_link_libraries(
        pc-datanode-pancake-engine
        PUBLIC
        Seastar::seastar
        stdc++_coroutine
)