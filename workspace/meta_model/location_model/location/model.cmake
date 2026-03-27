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
    ${CMAKE_CURRENT_LIST_DIR}/../location_basic_src/src/src/Location_.cpp
    ${MYSRC}
    CACHE STRING "module1 source files" FORCE)

# 2. 本模块需要的头文件搜索路径（可选）
set(MODULE1_INCLUDE_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/
    ${CMAKE_CURRENT_LIST_DIR}/
    ${CMAKE_CURRENT_LIST_DIR}/../location_basic_src/src/include

    # 通用公共头文件 (Log, Config, Fault 等)
    ${CMAKE_CURRENT_LIST_DIR}/../../../common
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/log
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/config_loader
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/monitor
    CACHE PATH "module1 include dirs" FORCE)

# 交叉编译（MDC/ARM）补充 sysroot include
if(NOT SCFI_SDK_TYPE STREQUAL "x86")
    if(NOT DEFINED MDC_SDK)
        set(MDC_SDK "/usr/local/mdc_sdk_llvm/dp_gea/mdc_cross_compiler")
    endif()
    list(APPEND MODULE1_INCLUDE_DIRS
        ${MDC_SDK}/sysroot/usr/include
        ${MDC_SDK}/sysroot/usr/include/adsfi/adb/include
        ${MDC_SDK}/sysroot/usr/include/adsfi/adsf/include
        ${MDC_SDK}/sysroot/usr/include/adsfi/arxml_include
        ${CMAKE_CURRENT_LIST_DIR}/../../../../tool_chain/mds_wks/MDS_MDC2501MidTracked/generated/includes/
    )
    set(MODULE1_INCLUDE_DIRS "${MODULE1_INCLUDE_DIRS}" CACHE PATH "module1 include dirs" FORCE)
endif()

# 3. 如果还需要链接第三方库
set(MODULE1_LIBS
    pthread
    dl
    glog
    yaml
    yaml-cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/proj4/lib/libProj4.so
    CACHE STRING "module1 link libraries" FORCE)

# SCFI 默认依赖里可能带 gflags，Location 不需要，先移除以免链接失败
if(DEFINED scfi_LIBRARIES)
    list(REMOVE_ITEM scfi_LIBRARIES gflags)
endif()

# Location 不依赖 gflags，避免交叉编译缺库报错
if(SCFI_SDK_TYPE STREQUAL "x86")
    set(MODULE1_LINK_DIRS
        /home/idriver/ap_adsfi/tools/avos_filespace_manager/lib
        CACHE PATH "module1 link dirs" FORCE)
else()
    if(NOT DEFINED MDC_SDK)
        set(MDC_SDK "/usr/local/mdc_sdk_llvm/dp_gea/mdc_cross_compiler")
    endif()
    set(MODULE1_LINK_DIRS
        ${MDC_SDK}/sysroot/usr/lib/mdc_vector
        ${MDC_SDK}/sysroot/usr/lib/mdc/base-plat
        CACHE PATH "module1 link dirs" FORCE)
endif()
