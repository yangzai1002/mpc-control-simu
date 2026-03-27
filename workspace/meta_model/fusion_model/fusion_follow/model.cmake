# 防止重复 include
include_guard(GLOBAL)

# 1. 本模块需要的源文件
set(MODULE1_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/adsfi_interface.cpp
    CACHE STRING "module1 source files" FORCE)

# 2. 本模块需要的头文件搜索路径（可选）
set(MODULE1_INCLUDE_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/src
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/perception_fusion_common/include
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/perception_fusion_follow/include
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/xappcommon/
    /usr/local/mdc_sdk_llvm/dp_gea/mdc_cross_compiler/sysroot/usr/include/opencv4/
    /usr/local/mdc_sdk_llvm/dp_gea/mdc_cross_compiler/sysroot/usr/include/pcl-1.11
    /usr/local/mdc_sdk_llvm/dp_gea/mdc_cross_compiler/sysroot/usr/include/pcl-1.11/pcl/
    CACHE PATH "module1 include dirs" FORCE)

# 2.1. 库文件搜索路径（如果 .so 文件不在标准库路径中）
# 注意：使用此文件的CMakeLists需要调用 link_directories(${MODULE1_LIB_DIRS})
set(MODULE1_LIB_DIRS
   # /usr/local/mdc_sdk_llvm/dp_gea/mdc_cross_compiler/sysroot/usr/lib64
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/perception_fusion_common/lib
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/perception_fusion_common/lib/20260125
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/proj4/lib
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/perception_fusion_follow/lib
    CACHE PATH "module1 library dirs" FORCE)

# 3. 如果还需要链接第三方库
set(MODULE1_LIBS
    pthread
    dl
    yaml
    yaml-cpp
    glog
    opencv_core
    opencv_imgproc
    opencv_imgcodecs
    opencv_highgui
    opencv_calib3d
    pcl_common
    pcl_segmentation
    pcl_sample_consensus
    pcl_kdtree
    pcl_search
    Proj4
    crypto
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/perception_fusion_follow/lib/libperception_fusion_follow.so
    # ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/perception_fusion_common/lib/libperception_fusion_common.so
    CACHE STRING "module1 link libraries" FORCE)

# 3.1. 需要完整路径链接的共享库（库名包含连字符，无法使用 -l 方式链接）
# 注意：使用此文件的CMakeLists需要在 target_link_libraries 中添加：
#       target_link_libraries(your_target ${MODULE1_LIBS} ${MODULE1_FULL_PATH_LIBS})
#set(MODULE1_FULL_PATH_LIBS
#    ${CMAKE_CURRENT_LIST_DIR}/../../../common/perception_fusion_common/lib/20260125/libfunction-perception-fusion-common.so
#    /usr/local/mdc_sdk_llvm/dp_gea/mdc_cross_compiler/sysroot/usr/lib64/libproj.so
#    CACHE STRING "module1 full path libraries" FORCE)
#-------------------------------------------------------------------------------------------
