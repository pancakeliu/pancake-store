# pancake-store comm CMakeLists.txt

file(
        GLOB_RECURSE
        COMM_SOURCES_AND_HEADERS
        "${PANCAKE_STORE_SRC_COMM_DIR}/*.cc"
        "${PANCAKE_STORE_SRC_COMM_DIR}/*.h"
)

include_directories(
        ${SEASTAR_INCLUDE}
)

#add_library(comm OBJECT ${COMM_SOURCES_AND_HEADERS})
#target_link_libraries(comm PUBLIC ${SEASTAR_LIB})
