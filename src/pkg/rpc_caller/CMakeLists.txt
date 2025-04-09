# pancake-store rpc-caller module cmake

file(
        GLOB_RECURSE
        PKG_RPC_CALLER_SOURCES_AND_HEADERS
        "${PC_PKG_RPC_CALLER_DIR}/*.h"
        "${PC_PKG_RPC_CALLER_DIR}/*.cc"
)

include_directories(
        ${SEASTAR_INCLUDE}
)

add_library(pc-pkg-rpc-caller OBJECT ${PKG_RPC_CALLER_SOURCES_AND_HEADERS})
target_link_libraries(
        pc-pkg-rpc-caller
        PUBLIC
        Seastar::seastar
)