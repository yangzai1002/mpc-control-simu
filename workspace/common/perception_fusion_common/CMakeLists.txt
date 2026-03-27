cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(MDC_SDK "/usr/local/mdc_sdk_llvm/dp_gea/mdc_cross_compiler" )

set(CMAKE_C_COMPILER "${MDC_SDK}/bin/clang")
set(CMAKE_CXX_COMPILER "${MDC_SDK}/bin/clang++")

#set(ara-exec_DIR ${MDC_SDK}/sysroot/usr/lib/cmake/exec/)
set(CMAKE_FIND_ROOT_PATH ${MDC_SDK}/sysroot/ ${CMAKE_FIND_ROOT_PATH})

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

SET (CMAKE_C_COMPILER_WORKS 1)
SET (CMAKE_CXX_COMPILER_WORKS 1)
ADD_DEFINITIONS(-DMDC_PRODUCTION_CORE)
project(function-perception-fusion-common)
set(APP_NAME function-perception-fusion-common)

SET(CMAKE_BUILD_TYPE RelWithDebInfo)

#把Debug和Release状态下默认的编译选项设置为空，避免和用户的设置重复
set(CMAKE_C_FLAGS_DEBUG "")
set(CMAKE_C_FLAGS_RELEASE "")
set(CMAKE_CXX_FLAGS_DEBUG "")
set(CMAKE_CXX_FLAGS_RELEASE  "")

IF (CMAKE_BUILD_TYPE MATCHES Debug)
    set(DEFAULT_COMPILE -std=c++14 -O2 -g3 -Wall -c -fmessage-length=0 -fPIC -fPIE -fstack-protector-all)
ELSEIF (CMAKE_BUILD_TYPE MATCHES Release)
    set(DEFAULT_COMPILE -std=c++14 -O3 -Wall -c -fmessage-length=0 -fPIC -fPIE -fstack-protector-all)
ENDIF()

add_compile_options(${DEFAULT_COMPILE})

set(MDC_ADSFI_SDK ${MDC_SDK}/sysroot/usr/include/adsfi)
set(MDC_ACLLIB_SDK ${MDC_SDK}/sysroot/usr/local/Ascend/runtime)
set(GODEL_LINK_LIBS "-lara_core -lara_com -lara_exec -lddscore -lddscpp  -lJsonParser -lKeyValueStorage -llogging -lsomeip -lvcc_ddsdriver -lvcc_someipdriver -lvrtf_vcc")
# 用户需要添加自己的头文件路径
INCLUDE_DIRECTORIES(
    include
    ${MDC_SDK}/sysroot/usr/include
    ${MDC_SDK}/sysroot/usr/include/adsfi/adb/include
    ${MDC_SDK}/sysroot/usr/include/adsfi/adsf/include
    ${MDC_SDK}/sysroot/usr/include/opencv4/
    ${MDC_SDK}/sysroot/usr/include/pcl-1.11/pcl/
    ${MDC_SDK}/sysroot/usr/include/pcl-1.11/
    SYSTEM ${MDC_SDK}/sysroot/usr/include/adsfi/arxml_include
    ${MDC_ACLLIB_SDK}/include
    ${MDC_ACLLIB_SDK}/include/acl
    ${MDC_ACLLIB_SDK}/include/acl/ops
    ${MDC_ACLLIB_SDK}/include/acl/dvpp
    ${MDC_ADSFI_SDK}
    ${MDC_ADSFI_SDK}/../dvpp

    ${CMAKE_CURRENT_LIST_DIR}/../
    ${CMAKE_CURRENT_LIST_DIR}/../../
    ${CMAKE_CURRENT_LIST_DIR}/../gsds_adapter/mdcplugin
    ${CMAKE_CURRENT_LIST_DIR}/../config_loader
    ${CMAKE_CURRENT_LIST_DIR}/../gsds_adapter/gsdsmsg/data_define/include
    ${CMAKE_CURRENT_LIST_DIR}/../../third_party/glog-0.6.0/include
    ${CMAKE_CURRENT_LIST_DIR}/../xappcommon
    ${CMAKE_CURRENT_LIST_DIR}/../shm_util
    ${CMAKE_CURRENT_LIST_DIR}/../gsds_adapter/gsdsmsg/msghandle/include
    ${CMAKE_CURRENT_LIST_DIR}/../../third_party/websocketpp

    ./include/
    ./include/perception_fusion_common/
    ./include/perception_fusion_common/radartracker
    # ./include/perception_fusion_common/radartracker
)


# 用户需要添加自己的库路径
link_directories(
    ${MDC_SDK}/sysroot/usr/lib/mdc_vector
    ${MDC_SDK}/sysroot/usr/lib/mdc/base-plat
    ${MDC_ACLLIB_SDK}/lib64/stub
    ${MDC_ACLLIB_SDK}/lib64
)

file(GLOB_RECURSE MYSRC
     CONFIGURE_DEPENDS        # CMake 3.12+ 推荐：源码树变动时自动重新扫描
     ./src/*)

add_library(${APP_NAME} SHARED
                        ${MYSRC})

# add_executable(${APP_NAME}
#                         main.cpp
#                         ${CXXDVPP_SOURCE} )



# 用户需要添加自己的库名称
target_link_libraries(${APP_NAME}
    ${GODEL_LINK_LIBS}
    adb
    # adsf
    ascendcl
    acl_dvpp_mpi
    fault_event
    iam
    easy_comm
    log
    logging
    # mdc_aiv
    ${MDC_SDK}/sysroot/usr/lib/libara_com.so
    adsfi_log
    rm
    ascend_hal
    securec
    opencv_core
    opencv_imgproc
    
    pcl_common
    pcl_features
    pcl_filters
    pcl_io
    pcl_io_ply
    pcl_kdtree
    pcl_keypoints
    pcl_ml
    pcl_octree
    
    
    pcl_recognition
    pcl_registration
    pcl_sample_consensus
    pcl_search
    pcl_segmentation
    pcl_stereo
    pcl_surface
    pcl_tracking
    
)

install(
    TARGETS ${APP_NAME} RUNTIME
    DESTINATION ${PROJECT_SOURCE_DIR}/bin
)
