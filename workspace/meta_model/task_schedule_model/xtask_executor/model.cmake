# 防止重复 include
include_guard(GLOBAL)

# 1. 本模块需要的源文件
set(MODULE1_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/adsfi_interface.cpp
    CACHE STRING "module1 source files" FORCE)

# 2. 本模块需要的头文件搜索路径（可选）
set(MODULE1_INCLUDE_DIRS
    ${MDC_SDK}/sysroot/usr/include/osqp
    ${CMAKE_CURRENT_LIST_DIR}/
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/osqpeigen-0.6.2/include/

    CACHE PATH "module1 include dirs" FORCE)

# 3. 如果还需要链接第三方库
set(MODULE1_LIBS
    pthread
    dl
    fmt
    glog
    yaml-cpp
    tinyxml2
    Proj4
    boost_filesystem
    boost_system
    CACHE STRING "module1 link libraries" FORCE)
