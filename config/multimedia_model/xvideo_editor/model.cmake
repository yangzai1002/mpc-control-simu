# 防止重复 include
include_guard(GLOBAL)

# 1. 本模块需要的源文件
set(MODULE1_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/color.cpp
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/VideoEditYUV420UV12.cpp
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/XImagePathPlan.cpp
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/XVideoEditor_Dvpp.cpp
    CACHE STRING "module1 source files" FORCE)

# 2. 本模块需要的头文件搜索路径（可选）
set(MODULE1_INCLUDE_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/params
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/protocol
    ${CMAKE_CURRENT_LIST_DIR}/
    /usr/local/mdc_sdk_llvm/dp_gea/mdc_cross_compiler/sysroot/usr/include/opencv4/
    CACHE PATH "module1 include dirs" FORCE)

# 3. 如果还需要链接第三方库
set(MODULE1_LIBS
    pthread
    dl
    yaml
    yaml-cpp
    glog
    opencv_core
    opencv_imgproc
    CACHE STRING "module1 link libraries" FORCE)
