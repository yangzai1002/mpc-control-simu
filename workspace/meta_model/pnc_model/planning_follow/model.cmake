# 防止重复 include
include_guard(GLOBAL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE MYSRC
      CONFIGURE_DEPENDS
      ${CMAKE_CURRENT_LIST_DIR}/../../../common/common_base/util/geotool.cc
      )

# 1. 本模块需要的源文件
set(MODULE1_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/adsfi_interface.cpp
    ${MYSRC}
    CACHE STRING "module1 source files" FORCE)

set(MDC_SDK "/usr/local/mdc_sdk_llvm/dp_gea/mdc_cross_compiler" )

# 2. 本模块需要的头文件搜索路径
set(MODULE1_INCLUDE_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/
    ${CMAKE_CURRENT_LIST_DIR}/

    # planning_follow_basic 库的头文件路径
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_follow_basic/include/BlockPerceptionData/include
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_follow_basic/include/XBridge/include
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_follow_basic/include/XBridgeFollow/include
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_follow_basic/include/XBusinessFollow/include
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_follow_basic/include/XConfigParamReader/include
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_follow_basic/include/XEnvironmentalCognition/include
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_follow_basic/include/XEnvironmentalCognition/src/include
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_follow_basic/include/XFollowPathPlanner/include
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_follow_basic/include/follow_debug/include
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_follow_basic/include/XFollowPathPlanner/src/include
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_follow_basic/include/XFollowPathSelector/include
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_follow_basic/include/XRefDirFollow/include
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_follow_basic/include/XRefRecFollow/include
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_follow_basic/include/XSpeedPlanFollow/include

    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/remote_control_basic/include

    ${MDC_SDK}/sysroot/usr/include/pcl-1.11/pcl/
    ${MDC_SDK}/sysroot/usr/include/pcl-1.11/
    ${MDC_SDK}/sysroot/usr/include/opencv4/

    CACHE PATH "module1 include dirs" FORCE)

set(MODULE1_LINK_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_follow_basic/lib
    CACHE PATH "module1 library dirs" FORCE)

# 3. 链接第三方库
# 注意：这里链接的是 planning_follow_basic
set(MODULE1_LIBS
    pthread
    dl
    visual
    glog
    yaml
    yaml-cpp

    pcl_common
    pcl_features
    pcl_filters
    pcl_io
    pcl_io_ply
    pcl_kdtree
    pcl_keypoints
    pcl_ml
    pcl_octree
    pcl_recognition
    pcl_registration
    pcl_sample_consensus
    pcl_search
    pcl_segmentation
    pcl_stereo
    pcl_surface
    pcl_tracking
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/proj4/lib/libProj4.so
    opencv_core
    opencv_imgproc
    opencv_imgcodecs
    opencv_highgui
    crypto
    planning_follow_basic 
    CACHE STRING "module1 link libraries" FORCE)