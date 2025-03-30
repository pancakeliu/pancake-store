# pancake-store datanode CMakeLists.txt

message("datanode cmake..")

# redundant module
#set(DATANODE_REDUNDANT_DIR ${PANCAKE_STORE_SRC_DATANODE_DIR}/redundant)
#add_subdirectory(${DATANODE_REDUNDANT_DIR})
#
## rpc caller module
#set(DATANODE_RPC_CALLER_DIR ${PANCAKE_STORE_SRC_DATANODE_DIR}/rpc_caller)
#add_subdirectory(${DATANODE_RPC_CALLER_DIR})
#
# service module
set(DATANODE_SERVICE_DIR ${PANCAKE_STORE_SRC_DATANODE_DIR}/service)
add_subdirectory(${DATANODE_SERVICE_DIR})

# device modlue
set(DATANODE_DEVICE_DIR ${PANCAKE_STORE_SRC_DATANODE_DIR}/device)
add_subdirectory(${DATANODE_DEVICE_DIR})

## storage module
#set(DATANODE_STORAGE_DIR ${PANCAKE_STORE_SRC_DATANODE_DIR}/storage)
#add_subdirectory(${DATANODE_STORAGE_DIR})
#
## transfer module
#set(DATANODE_TRANSFER_DIR ${PANCAKE_STORE_SRC_DATANODE_DIR}/transfer)
#add_subdirectory(${DATANODE_TRANSFER_DIR})
#
add_executable(pc-datanode ${PANCAKE_STORE_SRC_DATANODE_DIR}/main.cc)
add_dependencies(pc-datanode DepsFinished)

find_package (Seastar REQUIRED)

target_include_directories(
        pc-datanode
        PUBLIC
        ${SPDK_INCLUDE}
)

target_link_libraries(
        pc-datanode
        PUBLIC
        pc-datanode-service
        pc-rpc
        pc-proto
        Seastar::seastar
        protobuf
)