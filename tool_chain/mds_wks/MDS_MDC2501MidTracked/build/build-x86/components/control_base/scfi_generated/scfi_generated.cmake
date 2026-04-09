# 设定组件版本号
set(CMAKE_PROJECT_VERSION 1.0.0)

# 设定组件类型
set(SCFI_TYPE shared_library)

set(scfi_LIBRARIES ${PROJECT_NAME}_scfi_intf)

set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

# 校验组件名称
scfi_check(control_base)

# 自动引入头文件路径
scfi_include()

# 自动引入组件相关源文件
scfi_src()
# 根据ServiceInterface添加生成的源文件
set(scfi_SOURCES
    ${scfi_SOURCES}
    ${CMAKE_SOURCE_DIR}/generated/src/ara/adsfi/bodycommandserviceinterface_common.cpp

    ${CMAKE_SOURCE_DIR}/generated/src/ara/adsfi/vehicleinfomationserviceinterface_common.cpp

    ${CMAKE_SOURCE_DIR}/generated/src/ara/adsfi/faultdataserviceinterface_common.cpp

    ${CMAKE_SOURCE_DIR}/generated/src/adsfi/locationinfointf_common.cpp

    ${CMAKE_SOURCE_DIR}/generated/src/adsfi/planningresultintf_common.cpp
)

message(STATUS "Auto-generated source files found:
    ${CMAKE_SOURCE_DIR}/generated/src/ara/adsfi/bodycommandserviceinterface_common.cpp
    ${CMAKE_SOURCE_DIR}/generated/src/ara/adsfi/vehicleinfomationserviceinterface_common.cpp
    ${CMAKE_SOURCE_DIR}/generated/src/ara/adsfi/faultdataserviceinterface_common.cpp
    ${CMAKE_SOURCE_DIR}/generated/src/adsfi/locationinfointf_common.cpp
    ${CMAKE_SOURCE_DIR}/generated/src/adsfi/planningresultintf_common.cpp")

# 设定对接component.xml文件的接口库
add_library(${scfi_LIBRARIES} INTERFACE)
# 添加依赖库
target_link_libraries(${scfi_LIBRARIES} INTERFACE
    yaml
    pthread)

if(NOT DEFINED SCFI_DEP)
    set(SCFI_DEP "SCFI_DEP-NOTFOUND")
endif()

# 添加类型为mdc_sdk的依赖
list(APPEND MDC_SDK_DEP ara-log)

find_package(ara-log QUIET CMAKE_FIND_ROOT_PATH_BOTH)
if(ara-log_FOUND)
    message(STATUS "Found package dependency ara-log: ${ara-log_DIR}")
    scfi_get_lib(ara-log_LIBRARIES LIST "${ara-log_LIBRARIES}")
    include_directories(${ara-log_INCLUDE_DIRS})
    target_link_libraries(${scfi_LIBRARIES} INTERFACE ${ara-log_LIBRARIES})
else()
    message(WARNING "Dependency package ara-log was not found! \nCheck whether the SDK contains the ara-log package.")
    if(ara-log_NOT_FOUND_MESSAGE)
        message(FATAL_ERROR "The following imported targets are referenced, but are missing: ${ara-log_NOT_FOUND_MESSAGE}")
    endif()
endif()

list(APPEND MDC_SDK_DEP ara-per)

find_package(ara-per QUIET CMAKE_FIND_ROOT_PATH_BOTH)
if(ara-per_FOUND)
    message(STATUS "Found package dependency ara-per: ${ara-per_DIR}")
    scfi_get_lib(ara-per_LIBRARIES LIST "${ara-per_LIBRARIES}")
    include_directories(${ara-per_INCLUDE_DIRS})
    target_link_libraries(${scfi_LIBRARIES} INTERFACE ${ara-per_LIBRARIES})
else()
    message(WARNING "Dependency package ara-per was not found! \nCheck whether the SDK contains the ara-per package.")
    if(ara-per_NOT_FOUND_MESSAGE)
        message(FATAL_ERROR "The following imported targets are referenced, but are missing: ${ara-per_NOT_FOUND_MESSAGE}")
    endif()
endif()

list(APPEND MDC_SDK_DEP ara-com)

find_package(ara-com QUIET CMAKE_FIND_ROOT_PATH_BOTH)
if(ara-com_FOUND)
    message(STATUS "Found package dependency ara-com: ${ara-com_DIR}")
    scfi_get_lib(ara-com_LIBRARIES LIST "${ara-com_LIBRARIES}")
    include_directories(${ara-com_INCLUDE_DIRS})
    target_link_libraries(${scfi_LIBRARIES} INTERFACE ${ara-com_LIBRARIES})
else()
    message(WARNING "Dependency package ara-com was not found! \nCheck whether the SDK contains the ara-com package.")
    if(ara-com_NOT_FOUND_MESSAGE)
        message(FATAL_ERROR "The following imported targets are referenced, but are missing: ${ara-com_NOT_FOUND_MESSAGE}")
    endif()
endif()

list(APPEND MDC_SDK_DEP rm)

find_package(rm QUIET CMAKE_FIND_ROOT_PATH_BOTH)
if(rm_FOUND)
    message(STATUS "Found package dependency rm: ${rm_DIR}")
    scfi_get_lib(rm_LIBRARIES LIST "${rm_LIBRARIES}")
    include_directories(${rm_INCLUDE_DIRS})
    target_link_libraries(${scfi_LIBRARIES} INTERFACE ${rm_LIBRARIES})
else()
    message(WARNING "Dependency package rm was not found! \nCheck whether the SDK contains the rm package.")
    if(rm_NOT_FOUND_MESSAGE)
        message(FATAL_ERROR "The following imported targets are referenced, but are missing: ${rm_NOT_FOUND_MESSAGE}")
    endif()
endif()


if(NOT DEFINED MDC_SDK_DEP)
    set(MDC_SDK_DEP "MDC_SDK_DEP-NOTFOUND")
endif()


if(NOT DEFINED RPM_DEP)
    set(RPM_DEP "RPM_DEP-NOTFOUND")
endif()

# 定义安装路径信息变量
scfi_destinations()

# 定义安装信息
install(TARGETS ${scfi_LIBRARIES}
    EXPORT ${PROJECT_NAME}Targets
    RUNTIME DESTINATION ${SCFI_BIN}
    LIBRARY DESTINATION ${SCFI_LIB}
    ARCHIVE DESTINATION ${SCFI_SHARE}
    INCLUDES DESTINATION ${SCFI_INCLUDE})