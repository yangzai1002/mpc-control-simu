# 指定组件名称
project(XH265RtpPusher)

# 由SCFI提供的CMake宏，为特定于SCFI生成系统所必需的信息
scfi_package()

# 指定组件生成类型
# ${scfi_SOURCES}为SCFI解析component.xml文件后自动添加源文件集合的变量
# add_XX(${PROJECT_NAME} ${scfi_SOURCES} <cpp1>, <cpp2>, ..., <cppN>)

add_executable(${PROJECT_NAME} ${scfi_SOURCES})


# 指定目标引用哪些头文件路径
# ${scfi_INCLUDE_DIRS}为SCFI解析component.xml文件后自动添加头文件路径的变量
# target_include_directories(${PROJECT_NAME} PRIVATE ${scfi_INCLUDE_DIRS} <dir1>, <dir2>, ..., <dirN>)
target_include_directories(${PROJECT_NAME} PRIVATE ${scfi_INCLUDE_DIRS})

# 指定目标链接到哪些库
# ${scfi_LIBRARIES}为SCFI解析component.xml文件后自动链接库的变量
# target_link_libraries(${PROJECT_NAME} PUBLIC ${scfi_LIBRARIES} <lib1>, <lib2>, ... <libN>)
if(NOT ${SCFI_SDK_TYPE} STREQUAL "x86")
    target_link_libraries(${PROJECT_NAME} PRIVATE ascend_hal "-Wl,--no-as-needed")
else()
    add_definitions(-DSCFI_SDK_X86)
endif()
#  移除没必要要的符号引用
target_link_libraries(${PROJECT_NAME} PUBLIC ${scfi_LIBRARIES} "-Wl,--as-needed")
# 该命令用于初始化SCFI组件包安装/导出信息
scfi_install()