# 防止重复 include
include_guard(GLOBAL)


file( GLOB MYSRC ${CMAKE_CURRENT_LIST_DIR}/src/*)

# 1. 本模块需要的源文件
set(MODULE1_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/adsfi_interface.cpp
    ${MYSRC}
    CACHE STRING "module1 source files" FORCE)

# 2. 本模块需要的头文件搜索路径（可选）
set(MODULE1_INCLUDE_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/
    ${CMAKE_CURRENT_LIST_DIR}/
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/planning-common/
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/common_header
    ${CMAKE_CURRENT_LIST_DIR}/../../../common
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/log
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/config_loader

    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/remote_control_basic/include
    CACHE PATH "module1 include dirs" FORCE)


set(MODULE1_LINK_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/remote_control_basic/lib
    CACHE PATH "module1 library dirs" FORCE)

# 3. 如果还需要链接第三方库
set(MODULE1_LIBS
    pthread
    dl
    visual
    glog
    yaml
    yaml-cpp
    crypto
    remote_control_basic
    CACHE STRING "module1 link libraries" FORCE)
