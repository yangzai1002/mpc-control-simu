# 防止重复 include
include_guard(GLOBAL)

file(GLOB H265NALU_SRC ${CMAKE_CURRENT_LIST_DIR}/h265nalu/src/*)

# 1. 本模块需要的源文件
set(MODULE1_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/adsfi_interface.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/VideoEditYUV420UV12.cpp
    ${H265NALU_SRC}
    CACHE STRING "module1 source files" FORCE)

# 2. 本模块需要的头文件搜索路径（可选）
set(MODULE1_INCLUDE_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/
    ${CMAKE_CURRENT_LIST_DIR}/impl/
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/
    ${CMAKE_CURRENT_LIST_DIR}/h265nalu/include
    ${MDC_SDK}/sysroot/usr/include/opencv4/
    CACHE PATH "module1 include dirs" FORCE)

set(MODULE1_LIB_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/lib
    CACHE PATH "module1 library dirs" FORCE
)

# 添加库搜索路径到链接器
link_directories(${MODULE1_LIB_DIRS})

# 3. 如果还需要链接第三方库
set(MODULE1_LIBS
    pthread
    dl
    fmt
    glog
    opencv_core
    opencv_imgproc
    opencv_freetype
    freetype
    harfbuzz
    sqlite3
    yaml
    yaml-cpp
    CACHE STRING "module1 link libraries" FORCE)
