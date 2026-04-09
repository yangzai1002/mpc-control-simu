# 指定组件名称
project(Location)

# ==============================================
# 🟢 MDC 专用 ROS 配置（无 catkin，无报错）
# ==============================================
include_directories(
  ${scfi_INCLUDE_DIRS}
  /opt/ros/noetic/include
)

link_directories(
  /opt/ros/noetic/lib
)

# 由SCFI提供的CMake宏，为特定于SCFI生成系统所必需的信息
scfi_package()

# 指定组件生成类型
add_executable(${PROJECT_NAME} ${scfi_SOURCES})

# 指定目标引用哪些头文件路径
target_include_directories(${PROJECT_NAME} PRIVATE
  ${scfi_INCLUDE_DIRS}
  /opt/ros/noetic/include
)

# 指定目标链接到哪些库
if(NOT ${SCFI_SDK_TYPE} STREQUAL "x86")
    target_link_libraries(${PROJECT_NAME} PRIVATE ascend_hal "-Wl,--no-as-needed")
else()
    add_definitions(-DSCFI_SDK_X86)
endif()

# 链接 ROS 库（MDC 安全版）
target_link_libraries(${PROJECT_NAME} PUBLIC
  ${scfi_LIBRARIES}
  "-Wl,--as-needed"
)

# 该命令用于初始化SCFI组件包安装/导出信息
scfi_install()

