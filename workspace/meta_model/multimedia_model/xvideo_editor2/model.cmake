# 防止重复 include
include_guard(GLOBAL)

file(GLOB SRC_VISWARNING_BOX ${CMAKE_CURRENT_LIST_DIR}/src/XVisWariningBox/*.cpp)
file(GLOB SRC_VIDEOSELECTOR ${CMAKE_CURRENT_LIST_DIR}/src/XVideoSelector/*.cpp)
file(GLOB SRC_PUTIMAGE_INSHM ${CMAKE_CURRENT_LIST_DIR}/src/XPutImageInShm/*.cpp)
file(GLOB SRC_XDrawAssistLines ${CMAKE_CURRENT_LIST_DIR}/src/XDrawAssistLines/*.cpp)
file(GLOB SRC_XDrawObstacles ${CMAKE_CURRENT_LIST_DIR}/src/XDrawObstacles/*.cpp)
file(GLOB SRC_XImageLayout ${CMAKE_CURRENT_LIST_DIR}/src/XImageLayout/*.cpp)

# 1. 本模块需要的源文件
set(MODULE1_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/adsfi_interface.cpp
    ${SRC_VISWARNING_BOX}
    ${SRC_VIDEOSELECTOR}
    ${SRC_PUTIMAGE_INSHM}
    ${SRC_XDrawAssistLines}
    ${SRC_XDrawObstacles}
    ${SRC_XImageLayout}
    ${CMAKE_CURRENT_LIST_DIR}/src/color.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/VideoEditYUV420UV12.cpp
    CACHE STRING "module1 source files" FORCE)

# 2. 本模块需要的头文件搜索路径（可选）
set(MODULE1_INCLUDE_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/
    ${CMAKE_CURRENT_LIST_DIR}/impl/
    ${CMAKE_CURRENT_LIST_DIR}/
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${MDC_SDK}/sysroot/usr/include/opencv4/
    CACHE PATH "module1 include dirs" FORCE)

# 3. 如果还需要链接第三方库
set(MODULE1_LIBS
    pthread
    dl
    fmt
    opencv_imgproc
    opencv_core
    opencv_freetype
    freetype
    harfbuzz
    yaml
    yaml-cpp
    glog
    CACHE STRING "module1 link libraries" FORCE)
