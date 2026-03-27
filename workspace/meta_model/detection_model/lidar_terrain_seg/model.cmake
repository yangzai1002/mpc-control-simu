# 防止重复 include
include_guard(GLOBAL)
message("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%")

set(MDC_ACLLIB_SDK ${MDC_SDK}/sysroot/usr/local/Ascend/runtime)
message( ${MDC_ACLLIB_SDK})
add_definitions(-DENABLE_DVPP_INTERFACE)
# 1. 本模块需要的源文件
set(MODULE1_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/adsfi_interface.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/CalcPlane.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/terrain_seg.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/terrain_ogm_seg.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/terrain_range_seg.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/map_fusion.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../perception_common/common_func/basicfunc.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../perception_common/hungarian/hungarianAlg.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../perception_common/iou_cal/iou.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../perception_common/kalman/kalman_filter.cc
    ${CMAKE_CURRENT_LIST_DIR}/../../perception_common/utm/utm.cpp
    CACHE STRING "module1 source files" FORCE)
message(${CMAKE_CURRENT_LIST_DIR})

# 2. 本模块需要的头文件搜索路径（可选）
set(MODULE1_INCLUDE_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/
    ${MDC_ACLLIB_SDK}/include/
    ${MDC_ACLLIB_SDK}/include/acl
    ${MDC_ACLLIB_SDK}/include/acl/ops
    ${MDC_ACLLIB_SDK}/include/acl/dvpp
    ${MDC_SDK}/sysroot/usr/include/pcl-1.11/
    ${MDC_SDK}/sysroot/usr/include/opencv4/
    ${MDC_SDK}/sysroot/usr/include/opencv4/opencv2/core/opencl
    ${MDC_SDK}/sysroot/usr/include/driver/
    ${MDC_SDK}/sysroot/usr/include/
    ${MDC_SDK}/sysroot/usr/include/adsfi/adb/include/
    ${CMAKE_CURRENT_LIST_DIR}/../../perception_common/common_func/
    ${CMAKE_CURRENT_LIST_DIR}/../../perception_common/perception_header/
    ${CMAKE_CURRENT_LIST_DIR}/../../perception_common/datatype/
    ${CMAKE_CURRENT_LIST_DIR}/../../perception_common/hungarian/
    ${CMAKE_CURRENT_LIST_DIR}/../../perception_common/iou_cal/
    ${CMAKE_CURRENT_LIST_DIR}/../../perception_common/kalman/
    ${CMAKE_CURRENT_LIST_DIR}/../../perception_common/utm/
    CACHE PATH "module1 include dirs" FORCE)

set(MODULE1_LINLIBS
    ${MDC_SDK}/sysroot/usr/lib/mdc_vector
    ${MDC_SDK}/sysroot/usr/lib64
    ${MDC_SDK}/sysroot/usr/lib/mdc/base-plat
    ${MDC_ACLLIB_SDK}/lib64/stub
    ${MDC_ACLLIB_SDK}/lib64
)
# 3. 如果还需要链接第三方库
set(MODULE1_LIBS
    pthread
    rm
    visual
    yaml
    yaml-cpp
    pcl_io  
    pcl_common
    pcl_filters
    pcl_search
    pcl_segmentation
    opencv_core
    opencv_imgproc
    opencv_highgui
    opencv_video
    opencv_calib3d
    boost_filesystem
    glog
    ascendcl
    acl_dvpp_mpi
    acl_dvpp
    ascend_hal
    camera
    mdc_mbuf
    adb
    CACHE STRING "module1 link libraries" FORCE)
