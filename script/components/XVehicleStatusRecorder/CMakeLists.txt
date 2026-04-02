# 指定组件名称
project(XVehicleStatusRecorder)


scfi_package()

# 1. 解析 cmake.opt 拿到 MODULE_PATH / CONFIG_PATH
#include(/home/idriver/ap_adsfi/workspace/cmake/common.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../../../../../../ap_adsfi/workspace/cmake/common.cmake)

# 2. 包含模块，得到源文件列表和头文件路径
message("变量路径:"${MODULE_PATH})
if(DEFINED MODULE_PATH AND EXISTS "${MODULE_PATH}")
	include(${MODULE_PATH})
else()
	message(STATUS "变量未定义或文件不存在:     ${MODULE_PATH}")
endif()
 
# 指定组件生成类型
# ${scfi_SOURCES}为SCFI解析component.xml文件后自动添加源文件集合的变量
# add_XX(${PROJECT_NAME} ${scfi_SOURCES} <cpp1>, <cpp2>, ..., <cppN>)

add_executable(${PROJECT_NAME} ${scfi_SOURCES} ${MODULE1_SOURCES})


# 指定目标引用哪些头文件路径
# ${scfi_INCLUDE_DIRS}为SCFI解析component.xml文件后自动添加头文件路径的变量
# target_include_directories(${PROJECT_NAME} PRIVATE ${scfi_INCLUDE_DIRS} <dir1>, <dir2>, ..., <dirN>)
target_include_directories(${PROJECT_NAME} PRIVATE ${scfi_INCLUDE_DIRS} ${MODULE1_INCLUDE_DIRS})
target_link_directories(${PROJECT_NAME} PRIVATE ${MODULE1_LINK_DIRS})

# 5. 把 CONFIG_PATH 宏传进所有源文件（包括 foo.cpp）
target_compile_definitions(${PROJECT_NAME} PRIVATE
CONFIG_PATH="${CONFIG_PATH}")

# 指定目标链接到哪些库
# ${scfi_LIBRARIES}为SCFI解析component.xml文件后自动链接库的变量
# target_link_libraries(${PROJECT_NAME} PUBLIC ${scfi_LIBRARIES} <lib1>, <lib2>, ... <libN>)
if(NOT ${SCFI_SDK_TYPE} STREQUAL "x86")
target_link_libraries(${PROJECT_NAME} PRIVATE ascend_hal "-Wl,--no-as-needed")
else()
add_definitions(-DSCFI_SDK_X86)
endif()
#  移除没必要要的符号引用
target_link_libraries(${PROJECT_NAME} PUBLIC ${scfi_LIBRARIES} ${MODULE1_LIBS} yaml-cpp "-Wl,--as-needed")
# 该命令用于初始化SCFI组件包安装/导出信息
scfi_install()

