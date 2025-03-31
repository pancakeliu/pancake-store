# pancake-store tools cmake

# client dir
set(PANCAKE_STORE_TOOLS_CLIENT_DIR ${PANCAKE_STORE_TOOLS_DIR}/client)
add_subdirectory(${PANCAKE_STORE_TOOLS_CLIENT_DIR})

# bench dir
set(PANCAKE_STORE_TOOLS_BENCH_DIR ${PANCAKE_STORE_TOOLS_DIR}/bench)
add_subdirectory(${PANCAKE_STORE_TOOLS_BENCH_DIR})