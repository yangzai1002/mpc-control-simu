# 防止重复 include
include_guard(GLOBAL)

set(MDC_ACLLIB_SDK ${MDC_SDK}/sysroot/usr/local/Ascend/runtime)
message( ${MDC_ACLLIB_SDK})
add_definitions(-DENABLE_DVPP_INTERFACE)

# 1. 本模块需要的源文件
set(MODULE1_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/adsfi_interface.cpp
    ${CMAKE_CURRENT_LIST_DIR}/yolo_detection/src/yolo_detection.cpp
    ${CMAKE_CURRENT_LIST_DIR}/byte_track/src/BYTETracker.cpp
    ${CMAKE_CURRENT_LIST_DIR}/byte_track/src/kalmanFilter.cpp
    # 注意：根据你之前的文件列表，MD5的cpp在include里，建议移动到src下，或者直接引用
    ${CMAKE_CURRENT_LIST_DIR}/byte_track/src/STrack.cpp
    ${CMAKE_CURRENT_LIST_DIR}/byte_track/src/lapjv.cpp
    ${CMAKE_CURRENT_LIST_DIR}/byte_track/src/utils.cpp
    CACHE STRING "module1 source files" FORCE)

# 2. 本模块需要的头文件搜索路径（可选）

set(MODULE1_INCLUDE_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/config_loader/
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/perception_fusion_common/include/
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/gsds_adapter/gsdsmsg/data_define/include
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/monitor/
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/msgpack/
    ${CMAKE_CURRENT_LIST_DIR}/yolo_detection/include/
    ${CMAKE_CURRENT_LIST_DIR}/byte_track/include/
    ${CMAKE_CURRENT_LIST_DIR}/byte_track/src/
    ${MDC_ACLLIB_SDK}/include/
    ${MDC_ACLLIB_SDK}/include/acl
    ${MDC_ACLLIB_SDK}/include/acl/ops
    ${MDC_ACLLIB_SDK}/include/acl/dvpp
    ${MDC_SDK}/sysroot/usr/include/opencv4/
    ${MDC_SDK}/sysroot/usr/include/opencv4/opencv2/core/opencl
    ${MDC_SDK}/sysroot/usr/include/driver/
    ${MDC_SDK}/sysroot/usr/include/
    /usr/local/mdc_sdk_llvm/dp_gea/mdc_cross_compiler/sysroot/usr/include/opencv4/
    CACHE PATH "module1 include dirs" FORCE)

set(MODULE1_LINLIBS
    ${MDC_SDK}/sysroot/usr/lib/mdc_vector
    ${MDC_SDK}/sysroot/usr/lib/mdc/base-plat
    ${MDC_ACLLIB_SDK}/lib64/stub
    ${MDC_ACLLIB_SDK}/lib64
)

# 3. 如果还需要链接第三方库
set(MODULE1_LIBS
    pthread
    dl
    yaml
    yaml-cpp
    glog
    crypto
    visual
    pcl_io  
    pcl_common
    opencv_core
    opencv_imgcodecs
    opencv_imgproc
    opencv_highgui
    opencv_video
    opencv_calib3d
    ascendcl
    acl_dvpp_mpi
    acl_dvpp
    ascend_hal
    camera
    mdc_mbuf
    CACHE STRING "module1 link libraries" FORCE)
