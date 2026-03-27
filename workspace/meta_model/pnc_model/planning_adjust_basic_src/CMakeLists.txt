cmake_minimum_required(VERSION 3.10.2)

# ============================================
# 架构选择：-DARCH=x86 或 -DARCH=arm
# 示例：cmake -DARCH=x86 ..  或  cmake -DARCH=arm ..
# ============================================
if(NOT DEFINED ARCH)
    message(FATAL_ERROR "请指定架构: -DARCH=x86 或 -DARCH=arm")
endif()

if(NOT ARCH MATCHES "^(x86|arm)$")
    message(FATAL_ERROR "ARCH 必须是 x86 或 arm，当前值: ${ARCH}")
endif()

message(STATUS "编译架构: ${ARCH}")

# ============================================
# 通用设置
# ============================================
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

# -----------------------------------------------------------
# [配置] 项目名称
# -----------------------------------------------------------
project(planning_adjust_basic)
set(APP_NAME planning_adjust_basic)

if(NOT DEFINED CONFIG_PATH)
    set(CONFIG_PATH "/opt/mdc/config" CACHE STRING "Configuration file path")
endif()

SET(CMAKE_BUILD_TYPE RelWithDebInfo)

set(CMAKE_C_FLAGS_DEBUG "")
set(CMAKE_C_FLAGS_RELEASE "")
set(CMAKE_CXX_FLAGS_DEBUG "")
set(CMAKE_CXX_FLAGS_RELEASE "")

IF (CMAKE_BUILD_TYPE MATCHES Debug)
    set(DEFAULT_COMPILE -std=c++17 -O2 -g3 -Wall -c -fmessage-length=0 -fPIC -fPIE -fstack-protector-all)
ELSEIF (CMAKE_BUILD_TYPE MATCHES Release)
    set(DEFAULT_COMPILE -std=c++17 -O3 -Wall -c -fmessage-length=0 -fPIC -fPIE -fstack-protector-all)
ENDIF()

add_compile_options(${DEFAULT_COMPILE})

# ============================================
# 架构特定配置
# ============================================
if(ARCH STREQUAL "arm")
    # ==================== ARM 交叉编译配置 ====================
    set(CMAKE_SYSTEM_PROCESSOR arm)
    set(MDC_SDK "/usr/local/mdc_sdk_llvm/dp_gea/mdc_cross_compiler")
    
    set(CMAKE_C_COMPILER "${MDC_SDK}/bin/clang")
    set(CMAKE_CXX_COMPILER "${MDC_SDK}/bin/clang++")
    
    set(CMAKE_FIND_ROOT_PATH ${MDC_SDK}/sysroot/ ${CMAKE_FIND_ROOT_PATH})
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
    
    ADD_DEFINITIONS(-DMDC_PRODUCTION_CORE)
    
    # ARM 特有路径
    set(MDC_ADSFI_SDK ${MDC_SDK}/sysroot/usr/include/adsfi)
    set(MDC_ACLLIB_SDK ${MDC_SDK}/sysroot/usr/local/Ascend/runtime)
    
    # ARM 特有链接库
    set(GODEL_LINK_LIBS "-lara_core -lara_com -lara_exec -lddscore -lddscpp  -lJsonParser -lKeyValueStorage -llogging -lsomeip -lvcc_ddsdriver -lvcc_someipdriver -lvrtf_vcc")
    
    # ARM 特有 include 路径
    set(ARCH_SPECIFIC_INCLUDES
        ${MDC_SDK}/sysroot/usr/include
        ${MDC_SDK}/sysroot/usr/include/adsfi/adb/include
        ${MDC_SDK}/sysroot/usr/include/adsfi/adsf/include
        ${MDC_SDK}/sysroot/usr/include/pcl-1.11/pcl/
        ${MDC_SDK}/sysroot/usr/include/pcl-1.11/
        SYSTEM ${MDC_SDK}/sysroot/usr/include/adsfi/arxml_include
        ${MDC_ACLLIB_SDK}/include
        ${MDC_ACLLIB_SDK}/include/acl
        ${MDC_ACLLIB_SDK}/include/acl/ops
        ${MDC_ACLLIB_SDK}/include/acl/dvpp
        ${MDC_ADSFI_SDK}
        ${MDC_ADSFI_SDK}/../dvpp
        ${MDC_SDK}/sysroot/usr/include/osqp
    )
    
    # ARM 特有 link 路径
    set(ARCH_SPECIFIC_LINK_DIRS
        ${MDC_SDK}/sysroot/usr/lib/mdc_vector
        ${MDC_SDK}/sysroot/usr/lib/mdc/base-plat
        ${MDC_ACLLIB_SDK}/lib64/stub
        ${MDC_ACLLIB_SDK}/lib64
    )
    
    # ARM 特有链接库
    set(ARCH_SPECIFIC_LIBS
        ${GODEL_LINK_LIBS}
        adb
        ascendcl
        acl_dvpp_mpi
    )
    
else()
    # ==================== X86 本地编译配置 ====================
    set(CMAKE_SYSTEM_PROCESSOR x86_64)
    set(X86_LIB_PATH "/home/idriver/ap_adsfi/workspace/third_party_x86")
    set(MDC_SDK "${X86_LIB_PATH}/MDC_Ubuntu_X86")
    
    # X86 特有 include 路径
    set(ARCH_SPECIFIC_INCLUDES
        ${MDC_SDK}/mdc_platform/include
        
        ${X86_LIB_PATH}/osqp-v0.6.0/include
        ${X86_LIB_PATH}/osqp-v0.6.0/include/osqp
        ${X86_LIB_PATH}/pcl-1.11.1/include/pcl-1.11
        ${X86_LIB_PATH}/osqp-eigen-v0.6.2/include
        ${X86_LIB_PATH}/casadi-3.6.7/include
    )
    
    # X86 特有 link 路径
    set(ARCH_SPECIFIC_LINK_DIRS
        ${MDC_SDK}/mdc_platform/lib
        
        ${X86_LIB_PATH}/osqp-v0.6.0/lib
        ${X86_LIB_PATH}/pcl-1.11.1/lib
        ${X86_LIB_PATH}/osqp-eigen-v0.6.2/lib
        ${X86_LIB_PATH}/casadi-3.6.7/lib
    )
    
    # X86 特有链接库
    set(ARCH_SPECIFIC_LIBS
        crypto
    )
endif()

# ============================================
# 通用 Include 路径
# ============================================
INCLUDE_DIRECTORIES(
    include
    ${ARCH_SPECIFIC_INCLUDES}

    # 1. 本模块源码路径
    ${CMAKE_CURRENT_LIST_DIR}/src
    
    # BlockPerceptionData
    ${CMAKE_CURRENT_LIST_DIR}/src/BlockPerceptionData/include
    ${CMAKE_CURRENT_LIST_DIR}/src/BlockPerceptionData/src

    # XBridge
    ${CMAKE_CURRENT_LIST_DIR}/src/XBridge/include
    ${CMAKE_CURRENT_LIST_DIR}/src/XBridge/src

    # XBridgeLevelingAndHeading
    ${CMAKE_CURRENT_LIST_DIR}/src/XBridgeLevelingAndHeading/include
    ${CMAKE_CURRENT_LIST_DIR}/src/XBridgeLevelingAndHeading/src

    # XBusinessFollow
    ${CMAKE_CURRENT_LIST_DIR}/src/XBusinessFollow/include
    ${CMAKE_CURRENT_LIST_DIR}/src/XBusinessFollow/src

    # XLevelingAndHeadingAdjustment (包含内部头文件 HeadingAdjustment.h, Leveling.h)
    ${CMAKE_CURRENT_LIST_DIR}/src/XLevelingAndHeadingAdjustment/include
    ${CMAKE_CURRENT_LIST_DIR}/src/XLevelingAndHeadingAdjustment/src

    # 2. 外部 Common 与工具链依赖
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/remote_control_basic/include
    ${CMAKE_CURRENT_LIST_DIR}/../../
    ${CMAKE_CURRENT_LIST_DIR}/../../../../workspace/
    ${CMAKE_CURRENT_LIST_DIR}/../../../../workspace/third_party/
    ${CMAKE_CURRENT_LIST_DIR}/../../../../workspace/common/monitor/
    
    # 工具链生成的接口代码
    ${CMAKE_CURRENT_LIST_DIR}/../../../../tool_chain/mds_wks/MDS_MDC2501MidTracked/generated/includes/
    
    # Common 库
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/gsds_adapter/mdcplugin
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/config_loader
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/gsds_adapter/gsdsmsg/data_define/include
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/glog-0.6.0/include
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/xappcommon
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/shm_util
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/gsds_adapter/gsdsmsg/msghandle/include
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/websocketpp

    # Planning Common
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/planning-common/
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/planning-common/include
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/common_header
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/common_header/include
    ${CMAKE_CURRENT_LIST_DIR}/../../../common
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/avos_base
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/common_base
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/log

    # 第三方数学库
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/casadi-3.6.7/include/casadi
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/casadi-3.6.7/include/
)

# ============================================
# Link 路径
# ============================================
link_directories(
    ${ARCH_SPECIFIC_LINK_DIRS}
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/glog-0.6.0/lib
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/casadi-3.6.7/lib
)

# ============================================
# 源文件搜集
# ============================================
FILE(GLOB_RECURSE C_SOURCE src/*.c)
FILE(GLOB_RECURSE CXX_SOURCE src/*.cpp)

message(STATUS "CXX_SOURCE=${CXX_SOURCE}")

# ============================================
# 创建库
# ============================================
add_library(${APP_NAME} SHARED
    ${C_SOURCE} 
    ${CXX_SOURCE}
)

# 设置编译定义
target_compile_definitions(${APP_NAME} PRIVATE
    CONFIG_PATH="${CONFIG_PATH}")

# ============================================
# 链接库 (Planning 类模块通常需要较全的依赖)
# ============================================
target_link_libraries(${APP_NAME}
    ${ARCH_SPECIFIC_LIBS}
    pthread
    dl
    visual
    glog
    yaml
    yaml-cpp
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
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/proj4/lib/libProj4.so
    casadi
    crypto
    osqp
)

# ============================================
# 安装配置
# ============================================
set(LIB_OUT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../third_party/${APP_NAME}/lib")
set(INC_OUT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../third_party/${APP_NAME}/include")

# 安装库
install(TARGETS ${APP_NAME}
        LIBRARY DESTINATION ${LIB_OUT_DIR}
        ARCHIVE DESTINATION ${LIB_OUT_DIR}
        RUNTIME DESTINATION ${LIB_OUT_DIR})

# 安装头文件 (保持目录结构)
file(GLOB_RECURSE HEADERS
     "${CMAKE_CURRENT_LIST_DIR}/src/*.h"
     "${CMAKE_CURRENT_LIST_DIR}/src/*.hpp")

foreach(header ${HEADERS})
    file(RELATIVE_PATH rel_path "${CMAKE_CURRENT_LIST_DIR}/src" ${header})
    get_filename_component(rel_dir ${rel_path} DIRECTORY)
    install(FILES ${header}
            DESTINATION "${INC_OUT_DIR}/${rel_dir}")
endforeach()

message(STATUS "CMake 配置完成，输出目录: ${CMAKE_BINARY_DIR}")
