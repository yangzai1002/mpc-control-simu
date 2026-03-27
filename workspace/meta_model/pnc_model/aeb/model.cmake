# 防止重复 include
include_guard(GLOBAL)

# 自动扫描 src 目录下的所有源文件
file(GLOB_RECURSE MYSRC
      CONFIGURE_DEPENDS
      ${CMAKE_CURRENT_LIST_DIR}/../../../common/common_base/util/geotool.cc
      )

# 1. 本模块需要的源文件
set(MODULE1_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/adsfi_interface.cpp
    ${MYSRC}
    CACHE STRING "module1 source files" FORCE)

set(MDC_SDK "/usr/local/mdc_sdk_llvm/dp_gea/mdc_cross_compiler" )

# 2. 本模块需要的头文件搜索路径
set(MODULE1_INCLUDE_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/
    ${CMAKE_CURRENT_LIST_DIR}/
    
    # AEB 核心库头文件 (根据你提供的 aeb_basic_src tree 结构)
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/aeb_basic_src/include
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/aeb_basic_src/include/include
    
    # 通用公共头文件 (用于 Log, Config, Time 等)
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/planning-common/
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/common_header
    ${CMAKE_CURRENT_LIST_DIR}/../../../common
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/log
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/config_loader
    
    # PCL 库头文件
    ${MDC_SDK}/sysroot/usr/include/pcl-1.11/pcl/
    ${MDC_SDK}/sysroot/usr/include/pcl-1.11/
    
    CACHE PATH "module1 include dirs" FORCE)

set(MODULE1_LINK_DIRS
    # 仅链接 AEB 核心库
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/aeb_basic_src/lib
    CACHE PATH "module1 library dirs" FORCE)

# 3. 链接第三方库
set(MODULE1_LIBS
    pthread
    dl
    visual
    glog
    yaml
    yaml-cpp
    aeb_basic_src        # 核心逻辑库
    crypto
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/proj4/lib/libProj4.so
    CACHE STRING "module1 link libraries" FORCE)