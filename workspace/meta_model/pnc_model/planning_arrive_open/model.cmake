# 防止重复 include
include_guard(GLOBAL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE MYSRC
     CONFIGURE_DEPENDS   # CMake 3.12+ 推荐：源文件增删时自动重新扫描
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

    # 公共数据结构头文件路径
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/gsds_adapter/gsdsmsg/data_define/include
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/gsds_adapter/gsdsmsg/msghandle/include

    # planning_arrive_open_basic 库的头文件路径
    # 优先使用 third_party，如果不存在则使用源码路径
    # ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_arrive_open_basic/include/BlockPerceptionData/include
    # ${CMAKE_CURRENT_LIST_DIR}/../planning_arrive_open_basic_src/src/BlockPerceptionData/include
    # ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_arrive_open_basic/include/XBridge/include
    # ${CMAKE_CURRENT_LIST_DIR}/../planning_arrive_open_basic_src/src/XBridge/include
    # ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_arrive_open_basic/include/XBridgeArrive/include
    # ${CMAKE_CURRENT_LIST_DIR}/../planning_arrive_open_basic_src/src/XBridgeArrive/include
    # ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_arrive_open_basic/include/XBusinessArrive/include
    # ${CMAKE_CURRENT_LIST_DIR}/../planning_arrive_open_basic_src/src/XBusinessArrive/include
    # ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_arrive_open_basic/include/XConfigParamReader/include
    # ${CMAKE_CURRENT_LIST_DIR}/../planning_arrive_open_basic_src/src/XConfigParamReader/include
    # ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_arrive_open_basic/include/XEnvironmentalCognition/include
    # ${CMAKE_CURRENT_LIST_DIR}/../planning_arrive_open_basic_src/src/XEnvironmentalCognition/include
    # ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_arrive_open_basic/include/XBehaviorArrive/include
    # ${CMAKE_CURRENT_LIST_DIR}/../planning_arrive_open_basic_src/src/XBehaviorArrive/include
    # ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_arrive_open_basic/include/XFrenetCilqrPathPlanner/include
    # ${CMAKE_CURRENT_LIST_DIR}/../planning_arrive_open_basic_src/src/XFrenetCilqrPathPlanner/include
    # ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_arrive_open_basic/include/XSpeedPlanArrive/include
    # ${CMAKE_CURRENT_LIST_DIR}/../planning_arrive_open_basic_src/src/XSpeedPlanArrive/include

    #${CMAKE_CURRENT_LIST_DIR}/../../../third_party/remote_control_basic/include

    ${MDC_SDK}/sysroot/usr/include/pcl-1.11/pcl/
    ${MDC_SDK}/sysroot/usr/include/pcl-1.11/
    ${MDC_SDK}/sysroot/usr/include/opencv4/

    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/casadi-3.6.7/include/casadi
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/casadi-3.6.7/include/
    
    # ara/adsfi 工具链生成的头文件路径
    ${CMAKE_CURRENT_LIST_DIR}/../../../../tool_chain/mds_wks/MDS_MDC2501MidTracked/generated/includes/
    # 通用公共头文件路径
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/planning-common/
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/common_header
    ${CMAKE_CURRENT_LIST_DIR}/../../../common
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/log
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/config_loader
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/common_base
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/shm_util
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/monitor
    
    CACHE PATH "module1 include dirs" FORCE)

set(MODULE1_LINK_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/casadi-3.6.7/lib
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_arrive_open_basic/lib
    CACHE PATH "module1 library dirs" FORCE)

# 3. 链接第三方库
# 注意：这里链接的是 planning_arrive_open_basic
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
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party_arm/proj4/lib/libProj4.so
    opencv_core
    opencv_imgproc
    opencv_imgcodecs
    opencv_highgui
    # casadi
    # crypto
    # planning_arrive_open_basic
    CACHE STRING "module1 link libraries" FORCE)
