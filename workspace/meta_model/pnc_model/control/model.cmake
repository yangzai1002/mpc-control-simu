# 防止重复 include
include_guard(GLOBAL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 自动递归收集 src 下所有 .cpp .cc 源文件
file(GLOB_RECURSE MYSRC
     CONFIGURE_DEPENDS
     ${CMAKE_CURRENT_LIST_DIR}/controller/src/*.cpp
     ${CMAKE_CURRENT_LIST_DIR}/controller/src/*.cc
     ${CMAKE_CURRENT_LIST_DIR}/controller/src/*.c
)

# 如果你还需要 common 外部文件，就这样合并
file(GLOB_RECURSE COMMON_SRC
     ${CMAKE_CURRENT_LIST_DIR}/../../../common/common_base/util/geotool.cc
)

# 合并所有源文件
set(ALL_SRCS ${MYSRC} ${COMMON_SRC})

# file(GLOB_RECURSE MYSRC
#      CONFIGURE_DEPENDS   # CMake 3.12+ 推荐：源文件增删时自动重新扫描
#      ${CMAKE_CURRENT_LIST_DIR}/../../../common/common_base/util/geotool.cc)

# 1. 本模块需要的源文件
set(MODULE1_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/adsfi_interface.cpp
    ${ALL_SRCS}
    ${CMAKE_CURRENT_LIST_DIR}/controller/src/controller_node.cpp
    CACHE STRING "module1 source files" FORCE)

# 2. 本模块需要的头文件搜索路径（可选）
#message("00000000000000000000000000" ${MDC_SDK})
set(MODULE1_INCLUDE_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/
    ${CMAKE_CURRENT_LIST_DIR}/
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/controller/include/mpc_lateral_controller/qpoases
    ${CMAKE_CURRENT_LIST_DIR}/controller/include/mpc_lateral_controller/qp_solver
    ${CMAKE_CURRENT_LIST_DIR}/controller/include/mpc_lateral_controller/
    ${CMAKE_CURRENT_LIST_DIR}/controller/include/
    ${MDC_SDK}/sysroot/usr/include/adsfi/adb/include
    ${MDC_SDK}/adsfi/adb/include
    CACHE PATH "module1 include dirs" FORCE)

set(MODULE1_LINK_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/control_basic/lib
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
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/proj4/lib/libProj4.so
    #control_basic
    CACHE STRING "module1 link libraries" FORCE)
