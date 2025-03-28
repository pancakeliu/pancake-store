# pancake-store rpc CMakeLists.txt
file(
        GLOB_RECURSE
        RPC_SOURCES_AND_HEADERS
        "${PANCAKE_STORE_SRC_RPC_DIR}/*.cc"
        "${PANCAKE_STORE_SRC_RPC_DIR}/*.h"
)

include_directories(
        ${SEASTAR_INCLUDE}
)

add_library(rpc OBJECT ${RPC_SOURCES_AND_HEADERS})
target_link_libraries(rpc PUBLIC ${SEASTAR_LIB})
