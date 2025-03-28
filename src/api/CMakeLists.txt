# pancake-store api CMakeLists.txt
file(
        GLOB_RECURSE
        API_SOURCES_AND_HEADERS
        "${PANCAKE_STORE_SRC_API_DIR}/*.cc"
        "${PANCAKE_STORE_SRC_API_DIR}/*.h"
)

include_directories(
        ${SEASTAR_INCLUDE}
)

add_library(api OBJECT ${API_SOURCES_AND_HEADERS})
target_link_libraries(api PUBLIC ${SEASTAR_LIB})
