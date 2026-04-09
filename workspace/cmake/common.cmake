# common.cmake  --  CMake ≥3.5
# 功能：
# 1. 自动识别编译平台（ARM/X86）并建立对应的 third_party 软链接
# 2. 读取 ${CMAKE_SOURCE_DIR}/cmake.opt
# 3. 逐行解析：exeName, modulePath, configPath
# 4. 若当前 PROJECT_NAME 与 exeName 一致，则
#    - include(modulePath)
#    - 把 configPath 以宏形式塞进编译定义
# 5. 若找不到匹配行，给出警告但继续配置

cmake_minimum_required(VERSION 3.5)

# ============================================
# 平台自动检测与软链接设置
# ============================================

# 检测编译器类型来判断平台
# ARM 平台使用 Clang 交叉编译，X86 平台使用 GCC
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_C_COMPILER_ID MATCHES "Clang")
    set(DETECTED_ARCH "arm")
    message(STATUS "[Platform Detect] ARM platform detected (Compiler: Clang)")
    set(MDC_SDK "/usr/local/mdc_sdk_llvm/dp_gea/mdc_cross_compiler" )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_C_COMPILER_ID MATCHES "GNU")
    set(DETECTED_ARCH "x86")
    message(STATUS "[Platform Detect] X86 platform detected (Compiler: GCC)")
    link_libraries(rt)
    link_libraries(stdc++fs)

    set(CMAKE_SYSTEM_PROCESSOR x86_64)
    set(X86_LIB_PATH "/home/idriver/ap_auto/workspace/third_party_x86")
    set(MDC_SDK "${X86_LIB_PATH}/MDC_Ubuntu_X86")
    
    # OpenCV：X86 版本路径
    if(NOT DEFINED OPENCV_INCLUDE_DIR)
        set(OPENCV_INCLUDE_DIR ${X86_LIB_PATH}/opencv-4.5.5/include)
    endif()
    
    # X86 特有 include 路径
    include_directories(
        ${OPENCV_INCLUDE_DIR}
        ${MDC_SDK}/mdc_platform/include
        
        ${X86_LIB_PATH}/osqp-v0.6.0/include
        ${X86_LIB_PATH}/osqp-v0.6.0/include/osqp
        ${X86_LIB_PATH}/pcl-1.11.1/include/pcl-1.11
        ${X86_LIB_PATH}/opencv-4.5.5/include
        ${X86_LIB_PATH}/opencv-4.5.5/include/opencv4
        ${X86_LIB_PATH}/osqp-eigen-v0.6.2/include
        ${X86_LIB_PATH}/casadi-3.6.7/include
        /usr/include/eigen3/
    )
    
    # X86 特有 link 路径
    link_directories(
        ${MDC_SDK}/mdc_platform/lib
        
        ${X86_LIB_PATH}/osqp-v0.6.0/lib
        ${X86_LIB_PATH}/pcl-1.11.1/lib
        ${X86_LIB_PATH}/opencv-4.5.5/lib
        ${X86_LIB_PATH}/osqp-eigen-v0.6.2/lib
        ${X86_LIB_PATH}/casadi-3.6.7/lib
    )
else()
    # 如果无法通过编译器判断，尝试通过编译器路径或系统架构判断
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm|ARM|aarch64|AARCH64")
        set(DETECTED_ARCH "arm")
        message(STATUS "[Platform Detect] ARM platform detected (Processor: ${CMAKE_SYSTEM_PROCESSOR})")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64|i686|i386")
        set(DETECTED_ARCH "x86")
        message(STATUS "[Platform Detect] X86 platform detected (Processor: ${CMAKE_SYSTEM_PROCESSOR})")
    else()
        message(WARNING "[Platform Detect] Unknown platform, defaulting to x86")
        set(DETECTED_ARCH "x86")
    endif()
endif()

# 设置软链接路径
set(WORKSPACE_DIR "/home/cxy/control-simulation/ap_auto/workspace")
set(THIRD_PARTY_LINK "${WORKSPACE_DIR}/third_party")
set(THIRD_PARTY_TARGET "${WORKSPACE_DIR}/third_party_${DETECTED_ARCH}")

message(STATUS "[SoftLink] Target: ${THIRD_PARTY_TARGET}")
message(STATUS "[SoftLink] Link:   ${THIRD_PARTY_LINK}")

# 检查目标目录是否存在
if(NOT EXISTS "${THIRD_PARTY_TARGET}")
    message(WARNING "[SoftLink] Target directory does not exist: ${THIRD_PARTY_TARGET}")
    message(WARNING "[SoftLink] Please ensure third_party_${DETECTED_ARCH} is properly set up!")
else()
    # 检查当前软链接状态
    set(NEED_CREATE_LINK FALSE)
    
    if(EXISTS "${THIRD_PARTY_LINK}")
        if(IS_SYMLINK "${THIRD_PARTY_LINK}")
            # 读取当前指向
            file(READ_SYMLINK "${THIRD_PARTY_LINK}" CURRENT_TARGET)
            get_filename_component(CURRENT_TARGET_ABS "${CURRENT_TARGET}" ABSOLUTE BASE_DIR "${WORKSPACE_DIR}")
            
            if(CURRENT_TARGET_ABS STREQUAL THIRD_PARTY_TARGET)
                message(STATUS "[SoftLink] Already correctly linked to: ${CURRENT_TARGET}")
            else()
                message(STATUS "[SoftLink] Current link points to: ${CURRENT_TARGET}")
                message(STATUS "[SoftLink] Need to switch to: ${THIRD_PARTY_TARGET}")
                set(NEED_CREATE_LINK TRUE)
            endif()
        else()
            # 是普通目录或文件，需要备份
            set(BACKUP_NAME "third_party_backup_${DETECTED_ARCH}_$ENV{TIMESTAMP}")
            message(WARNING "[SoftLink] ${THIRD_PARTY_LINK} is not a symlink, backing up...")
            # 注意：CMake 不直接支持重命名，这里发出警告让用户手动处理
            message(WARNING "[SoftLink] Please manually backup ${THIRD_PARTY_LINK} and remove it")
            message(FATAL_ERROR "[SoftLink] Cannot proceed with existing non-symlink third_party directory")
        endif()
    else()
        message(STATUS "[SoftLink] Link does not exist, creating new...")
        set(NEED_CREATE_LINK TRUE)
    endif()
    
    # 需要创建/更新软链接
    if(NEED_CREATE_LINK)
        # 删除旧的软链接（如果存在）
        if(EXISTS "${THIRD_PARTY_LINK}")
            file(REMOVE "${THIRD_PARTY_LINK}")
            message(STATUS "[SoftLink] Removed old symlink")
        endif()
        
        # 创建新的软链接
        # 使用相对路径创建软链接，这样更便携
        file(CREATE_LINK 
            "third_party_${DETECTED_ARCH}" 
            "${THIRD_PARTY_LINK}" 
            SYMBOLIC
        )
        
        if(EXISTS "${THIRD_PARTY_LINK}")
            message(STATUS "[SoftLink] Successfully created: third_party -> third_party_${DETECTED_ARCH}")
        else()
            message(FATAL_ERROR "[workspaceSoftLink] Failed to create symlink!")
        endif()
    endif()
endif()


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include_directories(
    # /home/idriver/ap_adsfi/workspace/common
    ${CMAKE_CURRENT_LIST_DIR}/../
    ${CMAKE_CURRENT_LIST_DIR}/../common
    ${CMAKE_CURRENT_LIST_DIR}/../common/avos_base
    ${CMAKE_CURRENT_LIST_DIR}/../common/common_base
    ${CMAKE_CURRENT_LIST_DIR}/../common/cppcodec
    ${CMAKE_CURRENT_LIST_DIR}/../common/gsds_adapter/mdcplugin
    ${CMAKE_CURRENT_LIST_DIR}/../common/config_loader
    ${CMAKE_CURRENT_LIST_DIR}/../common/dvpp_interface
    ${CMAKE_CURRENT_LIST_DIR}/../common/gsds_adapter/gsdsmsg/data_define/include
    ${CMAKE_CURRENT_LIST_DIR}/../third_party/glog-0.6.0/include
    ${CMAKE_CURRENT_LIST_DIR}/../third_party/fmt/include
    ${CMAKE_CURRENT_LIST_DIR}/../common/xappcommon
    ${CMAKE_CURRENT_LIST_DIR}/../common/xzmqcommon
    ${CMAKE_CURRENT_LIST_DIR}/../common/shm_util
    ${CMAKE_CURRENT_LIST_DIR}/../common/gsds_adapter/gsdsmsg/msghandle/include
    ${CMAKE_CURRENT_LIST_DIR}/../third_party/websocketpp
    ${CMAKE_CURRENT_LIST_DIR}/../common/monitor
    ${CMAKE_CURRENT_LIST_DIR}/../third_party
    ${CMAKE_CURRENT_LIST_DIR}/../third_party/freetype/include
    ${CMAKE_CURRENT_LIST_DIR}/../common/planning-common
    ${CMAKE_CURRENT_LIST_DIR}/../common/common_header
    /usr/local/mdc_sdk_llvm/dp_gea/mdc_cross_compiler/sysroot/usr/include/adsfi/adb/include/
)

link_directories(
    ${CMAKE_CURRENT_LIST_DIR}/../third_party/glog-0.6.0/lib
    ${CMAKE_CURRENT_LIST_DIR}/../third_party/proj4/lib
    ${CMAKE_CURRENT_LIST_DIR}/../third_party/fmt/lib
    ${CMAKE_CURRENT_LIST_DIR}/../third_party/freetype/lib
)

# 1. 找到 cmake.opt
set(_opt_file "${CMAKE_CURRENT_LIST_DIR}/cmake.opt")
if(NOT EXISTS "${_opt_file}")
    message(WARNING "common.cmake: ${_opt_file} not found, skip parsing.")
    return()
endif()

# 2. 逐行读取
file(STRINGS "${_opt_file}" _lines ENCODING UTF-8)

set(_found_match FALSE)
foreach(_line IN LISTS _lines)
    string(REGEX REPLACE "^[ ]+" "" _line "${_line}")  # 去行首空格
    if(_line MATCHES "^#")      # 支持 # 注释
        continue()
    endif()
    # 按逗号拆成 3 份
    string(REPLACE "," ";" _parts "${_line}")
    list(LENGTH _parts _n)
    if(NOT _n EQUAL 3)
        continue()
    endif()
    list(GET _parts 0 _exe)
    list(GET _parts 1 _module)
    list(GET _parts 2 _cfg)

    string(STRIP "${_exe}"   _exe)
    string(STRIP "${_module}" _module)
    string(STRIP "${_cfg}"   _cfg)
    if(_exe STREQUAL PROJECT_NAME)
        set(MODULE_PATH "${_module}" CACHE PATH "path to module.cmake" FORCE)
        set(CONFIG_PATH "${_cfg}"   CACHE PATH "path to config file" FORCE)
        message(STATUS "common.cmake: matched PROJECT_NAME=${PROJECT_NAME}")
        message(STATUS "  -> module = ${MODULE_PATH}")
        message(STATUS "  -> config = ${CONFIG_PATH}")
        set(_found_match TRUE)
        # 不再直接 include 和加定义，只把路径留出来
        break()
    endif()

endforeach()

if(NOT _found_match)
    message(WARNING "common.cmake: no entry matched PROJECT_NAME=${PROJECT_NAME}")
    set(MODULE_PATH "" CACHE PATH "path to module.cmake" FORCE)
    set(CONFIG_PATH ""   CACHE PATH "path to config file" FORCE)
    set(MODULE1_SOURCES ""   CACHE STRING "path to config file" FORCE)
    set(MODULE1_INCLUDE_DIRS ""   CACHE STRING "path to config file" FORCE)
    set(MODULE1_LIB_DIRS ""   CACHE STRING "path to config file" FORCE)
    set(MODULE1_LIBS ""   CACHE STRING "path to config file" FORCE)

    include_directories(
        ${CMAKE_CURRENT_LIST_DIR}/../tools
    )

endif()
