# 防止重复 include
include_guard(GLOBAL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. 本模块需要的源文件
set(MODULE1_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/adsfi_interface.cpp
    CACHE STRING "module1 source files" FORCE)

# 2. 本模块需要的头文件搜索路径
set(MODULE1_INCLUDE_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/
    ${CMAKE_CURRENT_LIST_DIR}/
    CACHE PATH "module1 include dirs" FORCE)

# 3. 链接库目录（本模块不需要额外的链接目录）
set(MODULE1_LINK_DIRS
    CACHE PATH "module1 library dirs" FORCE)

# 4. 链接第三方库
set(MODULE1_LIBS
    pthread
    dl
    glog
    CACHE STRING "module1 link libraries" FORCE)
