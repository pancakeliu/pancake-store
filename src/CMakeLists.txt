# pancake-store src CMakeLists.txt

# proto directory
set(PANCAKE_STORE_SRC_API_DIR ${PANCAKE_STORE_SRC_DIR}/api)
add_subdirectory(${PANCAKE_STORE_SRC_API_DIR})

# comm directory
set(PANCAKE_STORE_SRC_COMM_DIR ${PANCAKE_STORE_SRC_DIR}/comm)
add_subdirectory(${PANCAKE_STORE_SRC_COMM_DIR})

# datanode directory
set(PANCAKE_STORE_SRC_DATANODE_DIR ${PANCAKE_STORE_SRC_DIR}/datanode)
add_subdirectory(${PANCAKE_STORE_SRC_DATANODE_DIR})