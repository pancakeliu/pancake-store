# datanode storage posix engine module cmake

file(
        GLOB_RECURSE
        STORAGE_POSIX_ENGINE_DIR_SOURCES_AND_HEADERS
        "${STORAGE_POSIX_ENGINE_DIR}/*.h"
        "${STORAGE_POSIX_ENGINE_DIR}/*.cc"
)

include_directories(
        ${SEASTAR_INCLUDE}
)

find_package (Seastar REQUIRED)

add_library(pc-datanode-posix-engine OBJECT ${STORAGE_POSIX_ENGINE_DIR_SOURCES_AND_HEADERS})
target_link_libraries(
        pc-datanode-posix-engine
        PUBLIC
        Seastar::seastar
)