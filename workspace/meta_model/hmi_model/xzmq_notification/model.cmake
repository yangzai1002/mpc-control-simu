# 防止重复 include
include_guard(GLOBAL)

# 1. 本模块需要的源文件
set(MODULE1_SOURCES
   ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/adsfi_interface.cpp
   ${CMAKE_CURRENT_LIST_DIR}/src/XMessageSend.cpp
   ${CMAKE_CURRENT_LIST_DIR}/src/Xzmq_notification3.cpp
    CACHE STRING "module1 source files" FORCE)

# 2. 本模块需要的头文件搜索路径（可选）
set(MDC_SDK "/usr/local/mdc_sdk_llvm/dp_gea/mdc_cross_compiler")

set(MODULE1_INCLUDE_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/idpware_onlycm/include
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/perception_fusion_common/include
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/
    ${MDC_SDK}/sysroot/usr/include/pcl-1.11/pcl/
    ${MDC_SDK}/sysroot/usr/include/pcl-1.11/
    ${MDC_SDK}/sysroot/usr/include/opencv4/
    CACHE PATH "module1 include dirs" FORCE)

# 3. 如果还需要链接第三方库
set(MODULE1_LIBS
    pthread
    dl
    glog
    zmq
    fmt
    proj
    ssl
    crypto
    yaml-cpp
    tinyxml2
    pcl_common
    pcl_kdtree
    pcl_search
    opencv_core
    opencv_imgproc
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/proj4/lib/libProj4.so
    CACHE STRING "module1 link libraries" FORCE)
