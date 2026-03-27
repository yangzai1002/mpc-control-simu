# 防止重复 include
include_guard(GLOBAL)

file( GLOB MYSRC ${CMAKE_CURRENT_LIST_DIR}/XH265Nalu/src/*)

# 1. 本模块需要的源文件
set(MODULE1_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/adsfi_interface.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/EncodedH265MediaMan.cpp
    ${MYSRC}
    CACHE STRING "module1 source files" FORCE)

# 2. 本模块需要的头文件搜索路径（可选）
set(MODULE1_INCLUDE_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/
    ${CMAKE_CURRENT_LIST_DIR}/
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/XH265Nalu/include
    CACHE PATH "module1 include dirs" FORCE)

# 3. 如果还需要链接第三方库
set(MODULE1_LIBS
    pthread
    dl
    glog
    fmt
    boost_system
    boost_filesystem
    CACHE STRING "module1 link libraries" FORCE)
