# pancake-store src CMakeLists.txt

# rpc directory
set(PANCAKE_STORE_SRC_RPC_DIR ${PANCAKE_STORE_SRC_DIR}/rpc)
add_subdirectory(${PANCAKE_STORE_SRC_RPC_DIR})

# comm directory
set(PANCAKE_STORE_SRC_COMM_DIR ${PANCAKE_STORE_SRC_DIR}/comm)
add_subdirectory(${PANCAKE_STORE_SRC_COMM_DIR})

# pkg directory
set(PANCAKE_STORE_SRC_PKG_DIR ${PANCAKE_STORE_SRC_DIR}/pkg)
add_subdirectory(${PANCAKE_STORE_SRC_PKG_DIR})

# datanode directory
set(PANCAKE_STORE_SRC_DATANODE_DIR ${PANCAKE_STORE_SRC_DIR}/datanode)
add_subdirectory(${PANCAKE_STORE_SRC_DATANODE_DIR})