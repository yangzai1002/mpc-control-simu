cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(MDC_SDK "/usr/local/mdc_sdk_llvm/dp_gea/mdc_cross_compiler" )
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_COMPILER "${MDC_SDK}/bin/clang")
set(CMAKE_CXX_COMPILER "${MDC_SDK}/bin/clang++")

set(CMAKE_FIND_ROOT_PATH ${MDC_SDK}/sysroot/ ${CMAKE_FIND_ROOT_PATH})

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)
add_definitions(-DMDC_PRODUCTION_CORE)

project(location_basic_src)
set(APP_NAME location_basic_src)

if(NOT DEFINED CONFIG_PATH)
    set(CONFIG_PATH "/opt/mdc/config" CACHE STRING "Configuration file path")
endif()

set(CMAKE_BUILD_TYPE RelWithDebInfo)

set(CMAKE_C_FLAGS_DEBUG "")
set(CMAKE_C_FLAGS_RELEASE "")
set(CMAKE_CXX_FLAGS_DEBUG "")
set(CMAKE_CXX_FLAGS_RELEASE  "")

if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(DEFAULT_COMPILE -std=c++17 -O2 -g3 -Wall -c -fmessage-length=0 -fPIC -fPIE -fstack-protector-all)
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    set(DEFAULT_COMPILE -std=c++17 -O3 -Wall -c -fmessage-length=0 -fPIC -fPIE -fstack-protector-all)
endif()

add_compile_options(${DEFAULT_COMPILE})

set(MDC_ADSFI_SDK ${MDC_SDK}/sysroot/usr/include/adsfi)

include_directories(
    ${MDC_SDK}/sysroot/usr/include
    ${MDC_SDK}/sysroot/usr/include/adsfi/adb/include
    ${MDC_SDK}/sysroot/usr/include/adsfi/adsf/include
    SYSTEM ${MDC_SDK}/sysroot/usr/include/adsfi/arxml_include
    ${MDC_ADSFI_SDK}
    ${MDC_ADSFI_SDK}/../dvpp

    # module sources
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/src/include

    # common includes
    ${CMAKE_CURRENT_LIST_DIR}/../../../common
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/log
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/config_loader
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/monitor
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/avos_base
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/common_base
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/common_header
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/common_header/include
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/gsds_adapter/gsdsmsg/data_define/include

    # generated adsfi includes
    ${CMAKE_CURRENT_LIST_DIR}/../../../../tool_chain/mds_wks/MDS_MDC2501MidTracked/generated/includes/

    # third party (glog headers)
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/glog-0.6.0/include
)

link_directories(
    ${MDC_SDK}/sysroot/usr/lib/mdc_vector
    ${MDC_SDK}/sysroot/usr/lib/mdc/base-plat
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/glog-0.6.0/lib
)

file(GLOB_RECURSE C_SOURCE src/*.c)
file(GLOB_RECURSE CXX_SOURCE src/*.cpp)

add_library(${APP_NAME} SHARED
    ${C_SOURCE}
    ${CXX_SOURCE}
)

# compile definitions
set_target_properties(${APP_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_compile_definitions(${APP_NAME} PRIVATE
    CONFIG_PATH="${CONFIG_PATH}")

# link libraries
# keep minimal dependencies for Location

target_link_libraries(${APP_NAME}
    pthread
    dl
    glog
    yaml
    yaml-cpp
)

# library output (optional)
set(LIB_OUT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../third_party/${APP_NAME}/lib")
set(INC_OUT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../third_party/${APP_NAME}/include")

install(TARGETS ${APP_NAME}
        LIBRARY DESTINATION ${LIB_OUT_DIR}
        ARCHIVE DESTINATION ${LIB_OUT_DIR}
        RUNTIME DESTINATION ${LIB_OUT_DIR})

file(GLOB_RECURSE HEADERS_SRC
     "${CMAKE_CURRENT_LIST_DIR}/src/*.h"
     "${CMAKE_CURRENT_LIST_DIR}/src/*.hpp")

foreach(header ${HEADERS_SRC})
    file(RELATIVE_PATH rel_path "${CMAKE_CURRENT_LIST_DIR}" ${header})
    get_filename_component(rel_dir ${rel_path} DIRECTORY)
    install(FILES ${header}
            DESTINATION "${INC_OUT_DIR}/${rel_dir}")
endforeach()
