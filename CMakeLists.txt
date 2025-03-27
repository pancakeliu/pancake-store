cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(pancake-store VERSION 1.0)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE release)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pipe -W -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -fPIC -fno-omit-frame-pointer")

if(CMAKE_BUILD_TYPE MATCHES debug-asan)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fsanitize=address -static-libasan")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPANCAKE_STORE_DEBUG -DDEBUG_ASAN")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-inline -fprofile-instr-generate -fcoverage-mapping")
    endif()
elseif(CMAKE_BUILD_TYPE MATCHES debug)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPANCAKE_STORE_DEBUG")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-inline -fprofile-instr-generate -fcoverage-mapping")
    endif()
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -DNDEBUG")
endif()

execute_process(
        COMMAND git rev-parse --short HEAD
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(GIT_EXECUTABLE /usr/bin/git)
execute_process(COMMAND
        "${GIT_EXECUTABLE}" rev-parse --short HEAD
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE GIT_SHA1
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND
        "${GIT_EXECUTABLE}" describe --tags
        OUTPUT_VARIABLE GIT_TAG
        OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND
        "${GIT_EXECUTABLE}" rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE GIT_BRANCH
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND
        "${GIT_EXECUTABLE}" log -1 --format=%ad --date=local
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE GIT_DATE
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

# configure_file("${CMAKE_SOURCE_DIR}/src/comm/version.cc.in" "${CMAKE_SOURCE_DIR}/src/comm/version.cc" @ONLY)

if (CMAKE_BUILD_MODULE MATCHES deps)
    # third party directory
    set(PANCAKE_STORE_THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/third_party)
    set(PANCAKE_STORE_DEPS_DIR ${CMAKE_SOURCE_DIR}/deps)
    add_subdirectory(${PANCAKE_STORE_THIRD_PARTY_DIR})
else()
    message("cmake to build pancake-store!!!!")

    # third party directory
    set(PANCAKE_STORE_THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/third_party)
    set(PANCAKE_STORE_DEPS_DIR ${CMAKE_SOURCE_DIR}/deps)
    add_subdirectory(${PANCAKE_STORE_THIRD_PARTY_DIR})

    # src directory
    set(PANCAKE_STORE_SRC_DIR ${CMCMAKE_SOURCE_DIR}/src)
    add_subdirectory(${PANCAKE_STORE_SRC_DIR})
endif ()