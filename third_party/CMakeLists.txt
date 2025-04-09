# add external project

# enable ExternalProject CMake module
include(ExternalProject)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
    set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)
endif()

if(CMAKE_BUILD_CORES)
    set(CPU_CORES ${CMAKE_BUILD_CORES})
else()
    # get cpu cores
    execute_process(
            COMMAND bash -c "nproc --all"
            OUTPUT_VARIABLE CPU_CORES
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()

# third paths

# seastar lib path
set(SEASTAR_SOURCE_DIR ${PANCAKE_STORE_THIRD_PARTY_DIR}/seastar)
set(SEASTAR_LIB_DIR ${PANCAKE_STORE_DEPS_DIR}/seastar)
set(SEASTAR_INCLUDE ${SEASTAR_LIB_DIR}/include CACHE STRING "seastar lib include dir")
set(SEASTAR_STATIC_LIB ${SEASTAR_LIB_DIR}/lib CACHE STRING "seastar static lib dir")

# gtest lib path
set(GTEST_SOURCE_DIR ${PANCAKE_STORE_THIRD_PARTY_DIR}/googletest)
set(GTEST_LIB_DIR ${PANCAKE_STORE_DEPS_DIR}/googletest)
set(GTEST_INCLUDE ${GTEST_LIB_DIR}/include CACHE STRING "gtest lib include dir")
set(GTEST_STATIC_LIB ${GTEST_LIB_DIR}/lib CACHE STRING "gtest static lib dir")

# spdk lib path
set(SPDK_SOURCE_DIR ${PANCAKE_STORE_THIRD_PARTY_DIR}/spdk)
set(SPDK_LIB_DIR ${PANCAKE_STORE_DEPS_DIR}/spdk)
set(SPDK_INCLUDE ${SPDK_LIB_DIR}/include CACHE STRING "spdk lib include dir")
set(SPDK_STATIC_LIB ${SPDK_LIB_DIR}/lib CACHE STRING "spdk static lib dir")

# isal lib path
set(ISAL_SOURCE_DIR ${PANCAKE_STORE_THIRD_PARTY_DIR}/isa-l)
set(ISAL_LIB_DIR ${PANCAKE_STORE_DEPS_DIR}/isal)
set(ISAL_INCLUDE ${ISAL_LIB_DIR}/include CACHE STRING "isal include dir")
set(ISAL_STATIC_LIB ${ISAL_LIB_DIR}/lib CACHE STRING "isal static lib dir")

if (CMAKE_BUILD_MODULE MATCHES deps)
    # install isal lib
    ExternalProject_Add(
            isal
            SOURCE_DIR ${ISAL_SOURCE_DIR}
            BINARY_DIR ${ISAL_SOURCE_DIR}
            CONFIGURE_COMMAND bash ${ISAL_SOURCE_DIR}/autogen.sh && ${ISAL_SOURCE_DIR}/configure --prefix=${ISAL_LIB_DIR} --libdir=${ISAL_LIB_DIR}/lib
            BUILD_COMMAND make -j${CPU_CORES}
            INSTALL_COMMAND make install
    )

    # install gtest lib
    ExternalProject_Add(
            googletest
            SOURCE_DIR ${GTEST_SOURCE_DIR}
            CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${GTEST_LIB_DIR}
            BUILD_COMMAND ${CMAKE_COMMAND} --build . -j ${CPU_CORES}
            INSTALL_COMMAND ${CMAKE_COMMAND} --build . --target install
            DEPENDS isal
    )

    # include seastar lib
    ExternalProject_Add(
            seastar
            SOURCE_DIR ${SEASTAR_SOURCE_DIR}
            BINARY_DIR ${SEASTAR_SOURCE_DIR}
            CONFIGURE_COMMAND cd ${SEASTAR_SOURCE_DIR} && sudo bash install-dependencies.sh && ./configure.py --mode=release --prefix=${SEASTAR_LIB_DIR} --without-tests --without-apps --without-demos --c++-standard=20
            BUILD_COMMAND cd ${SEASTAR_SOURCE_DIR} && ninja -C build/debug -j ${CPU_CORES}
            INSTALL_COMMAND cd ${SEASTAR_SOURCE_DIR} && ninja -C build/debug install
            DEPENDS googletest
    )

    # include spdk lib
    ExternalProject_Add(
            spdk
            SOURCE_DIR ${SPDK_SOURCE_DIR}
            BINARY_DIR ${SPDK_SOURCE_DIR}
            CONFIGURE_COMMAND cd ${SPDK_SOURCE_DIR} && sudo bash ./scripts/pkgdep.sh && bash ./configure --prefix=${SPDK_LIB_DIR}
            BUILD_COMMAND make -j${CPU_CORES}
            INSTALL_COMMAND make install
            DEPENDS seastar
    )

    add_custom_target(DepsFinished
            COMMAND echo "all third libraries install finished."
            DEPENDS isal seastar googletest spdk
    )
else ()
    add_custom_target(DepsFinished
            COMMAND echo "all third libraries install finished."
    )
endif ()

# set static lib
set(SEASTAR_LIB ${SEASTAR_STATIC_LIB}/libseastar.a CACHE STRING "seastar lib path")
set(SPDK_LIB ${SPDK_STATIC_LIB}/libspdk.a CACHE STRING "spdk lib path")
set(ISAL_LIB ${ISAL_STATIC_LIB}/libisal.a CACHE STRING "isal lib path")
set(GTEST_LIB ${GTEST_STATIC_LIB}/libgtest.a CACHE STRING "gtest lib path")
set(GTEST_MAIN_LIB ${GTEST_STATIC_LIB}/libgtest_main.a CACHE STRING "gtest_main lib path")
set(GTEST_MOCK_LIB ${GTEST_STATIC_LIB}/libgmock.a CACHE STRING "gtest_mock lib path")