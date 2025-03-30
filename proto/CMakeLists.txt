# pancake-store proto CMakeList.txt

# gen pancake-store prot
execute_process(
        COMMAND bash gen.sh
        WORKING_DIRECTORY ${PANCAKE_STORE_PROTO_DIR}
)

file(
        GLOB_RECURSE
        PROTO_SOURCES_AND_HEADERS
        "${PANCAKE_STORE_PROTO_DIR}/*.h"
        "${PANCAKE_STORE_PROTO_DIR}/*.cc"
)
add_library(pc-proto OBJECT ${PROTO_SOURCES_AND_HEADERS})
