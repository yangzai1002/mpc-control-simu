# 防止重复 include
include_guard(GLOBAL)

# 自动扫描源文件
file(GLOB_RECURSE MYSRC
      CONFIGURE_DEPENDS
      ${CMAKE_CURRENT_LIST_DIR}/../../../common/common_base/util/geotool.cc
      )

# 1. 本模块需要的源文件
set(MODULE1_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/adsfi_interface.cpp
    CACHE STRING "module1 source files" FORCE)

# ========================================================
# 2. 链接路径 (LINK_DIRS) 逻辑 - 核心修复区
# ========================================================

# 先定义基础路径：包含你的 DR 核心库
set(TMP_LINK_DIRS "/home/idriver/ap_adsfi/workspace/third_party/dr_basic_src/lib")

if(SCFI_SDK_TYPE STREQUAL "x86")
    # 追加 x86 平台路径
    list(APPEND TMP_LINK_DIRS "/home/idriver/ap_adsfi/tools/avos_filespace_manager/lib")
else()
    # 追加 MDC/ARM 平台路径
    if(NOT DEFINED MDC_SDK)
        set(MDC_SDK "/usr/local/mdc_sdk_llvm/dp_gea/mdc_cross_compiler")
    endif()
    list(APPEND TMP_LINK_DIRS 
        ${MDC_SDK}/sysroot/usr/lib/mdc_vector
        ${MDC_SDK}/sysroot/usr/lib/mdc/base-plat
        ${MDC_SDK}/sysroot/usr/lib
        ${MDC_SDK}/sysroot/usr/lib64
    )
endif()

# 统一写入全局缓存，确保不被冲掉
set(MODULE1_LINK_DIRS "${TMP_LINK_DIRS}" CACHE PATH "module1 link dirs" FORCE)

# 调试打印：编译时请检查终端输出是否包含上述所有路径
message(STATUS ">>>> [CONFIRM] Odometry Link Dirs: ${MODULE1_LINK_DIRS}")


# ========================================================
# 3. 头文件路径 (INCLUDE_DIRS)
# ========================================================
set(MODULE1_INCLUDE_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/
    ${CMAKE_CURRENT_LIST_DIR}/
    # 增加 DR 核心库安装后的头文件路径
    /home/idriver/ap_adsfi/workspace/third_party/dr_basic_src/include
    /home/idriver/ap_adsfi/workspace/meta_model/location_model/dr_basic_src/src/include

    # 通用公共头文件
    ${CMAKE_CURRENT_LIST_DIR}/../../../common
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/log
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/config_loader
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/monitor
    CACHE PATH "module1 include dirs" FORCE)

# 交叉编译补充头文件
if(NOT SCFI_SDK_TYPE STREQUAL "x86")
    list(APPEND MODULE1_INCLUDE_DIRS
        ${MDC_SDK}/sysroot/usr/include
        ${MDC_SDK}/sysroot/usr/include/adsfi/adb/include
        ${MDC_SDK}/sysroot/usr/include/adsfi/adsf/include
        ${MDC_SDK}/sysroot/usr/include/adsfi/arxml_include
        ${CMAKE_CURRENT_LIST_DIR}/../../../../tool_chain/mds_wks/MDS_MDC2501MidTracked/generated/includes/
    )
    set(MODULE1_INCLUDE_DIRS "${MODULE1_INCLUDE_DIRS}" CACHE PATH "module1 include dirs" FORCE)
endif()


# ========================================================
# 4. 链接库列表 (LIBS)
# ========================================================
set(MODULE1_LIBS
    pthread
    dl
    glog
    yaml
    yaml-cpp
    dr_basic_src  # 链接你的核心库 libdr_basic_src.so
    CACHE STRING "module1 link libraries" FORCE)

# 处理 gflags 逻辑
if(DEFINED scfi_LIBRARIES)
    list(REMOVE_ITEM scfi_LIBRARIES gflags)
endif()

set(GFLAGS_LIB "")
# 遍历当前已有的链接路径寻找 gflags
foreach(_dir ${MODULE1_LINK_DIRS})
    if(EXISTS "${_dir}/libgflags.so")
        set(GFLAGS_LIB "${_dir}/libgflags.so")
        break()
    elseif(EXISTS "${_dir}/libgflags.so.2.2")
        set(GFLAGS_LIB "${_dir}/libgflags.so.2.2")
        break()
    endif()
endforeach()

if(GFLAGS_LIB)
    list(APPEND MODULE1_LIBS ${GFLAGS_LIB})
else()
    message(WARNING "gflags library not found in MODULE1_LINK_DIRS")
endif()

# 最终写入全局链接库列表
set(MODULE1_LIBS "${MODULE1_LIBS}" CACHE STRING "module1 link libraries" FORCE)
