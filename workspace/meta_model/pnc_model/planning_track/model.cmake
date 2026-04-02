# 防止重复 include
include_guard(GLOBAL)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE MYSRC
     CONFIGURE_DEPENDS
     ${CMAKE_CURRENT_LIST_DIR}/Emplanner/frenet/*.cpp
     ${CMAKE_CURRENT_LIST_DIR}/Emplanner/map_processor/*.cpp
     ${CMAKE_CURRENT_LIST_DIR}/Emplanner/path_planner/*.cpp
     ${CMAKE_CURRENT_LIST_DIR}/Emplanner/reference_line/*.cpp
     ${CMAKE_CURRENT_LIST_DIR}/Emplanner/speed_planner/*.cpp
     ${CMAKE_CURRENT_LIST_DIR}/Emplanner/trajectory/*.cpp
     ${CMAKE_CURRENT_LIST_DIR}/planner_manager.cpp
     ${CMAKE_CURRENT_LIST_DIR}/planner_interface.cpp
)

# 如果你还需要 common 外部文件，就这样合并
file(GLOB_RECURSE COMMON_SRC
     ${CMAKE_CURRENT_LIST_DIR}/../../../common/common_base/util/geotool.cc
)

# 合并所有源文件
set(ALL_SRCS ${MYSRC} ${COMMON_SRC})

# 1. 本模块需要的源文件
set(MODULE1_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/adsfi_interface.cpp
    ${MYSRC}
    CACHE STRING "module1 source files" FORCE)

set(MDC_SDK "/usr/local/mdc_sdk_llvm/dp_gea/mdc_cross_compiler" )

# 2. 本模块需要的头文件搜索路径（可选）
set(MODULE1_INCLUDE_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/
    ${CMAKE_CURRENT_LIST_DIR}/
    ${MDC_SDK}/sysroot/usr/include/adsfi/adb/include
    # ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_track_basic/include/BlockPerceptionData/include
    # ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_track_basic/include/XBusinessTrack/include
    # ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_track_basic/include/XEnvironmentalCognition/include
    # ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_track_basic/include/XBehaviorComplexTrack/include
    # ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_track_basic/include/XCasadiLocalOptimize/include
    # ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_track_basic/include/XHybridAstar/include
    # ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_track_basic/include/XBridge/include
    # ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_track_basic/include/XConfigParamReader/include
    # ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_track_basic/include/XSpeedPlanTrack/include
    # ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_track_basic/include/XBridgeTrack/include
    # ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_track_basic/include/XDynamicProgramming/include
    # ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_track_basic/include/XTurnAround/include

    #${CMAKE_CURRENT_LIST_DIR}/../../../third_party/remote_control_basic/include

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


    ${MDC_SDK}/sysroot/usr/include/pcl-1.11/pcl/
    ${MDC_SDK}/sysroot/usr/include/pcl-1.11/
    ${MDC_SDK}/sysroot/usr/include/opencv4/
    ${MDC_SDK}/sysroot/usr/include/osqp/
    ${CMAKE_CURRENT_LIST_DIR}/Emplanner/
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/casadi-3.6.7/include/casadi
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/osqpeigen-0.6.2/include/
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/casadi-3.6.7/include/
    CACHE PATH "module1 include dirs" FORCE)


# link_directories(
#     ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/casadi-3.6.7/lib
#     )
set(MODULE1_LINK_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/casadi-3.6.7/lib
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/planning_track_basic/lib
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/osqpeigen-0.6.2/lib/
    CACHE PATH "module1 library dirs" FORCE)
# 3. 如果还需要链接第三方库
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
    OsqpEigen
    osqp
    casadi
    crypto
    #planning_track_basic
    CACHE STRING "module1 link libraries" FORCE)
