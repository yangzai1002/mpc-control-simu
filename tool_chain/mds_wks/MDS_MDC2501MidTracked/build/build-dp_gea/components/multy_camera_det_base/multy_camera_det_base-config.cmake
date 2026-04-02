# ===================================================================================
#  The multy_camera_det_base CMake configuration file
#
#             ** File generated automatically, do not modify **
#
#  Usage from an external project:
#    In your CMakeLists.txt, add these lines:
#
#    find_package(multy_camera_det_base REQUIRED)
#    target_include_directories(MY_TARGET_NAME ${multy_camera_det_base_INCLUDE_DIRS})
#    target_link_libraries(MY_TARGET_NAME ${multy_camera_det_base_LIBRARIES})
#
#    This file will define the following variables:
#      - multy_camera_det_base_LIBRARIES                : The list of all imported targets for multy_camera_det_base modules.
#      - multy_camera_det_base_INCLUDE_DIRS             : The multy_camera_det_base include directories.
#      - multy_camera_det_base_VERSION                  : The version of this multy_camera_det_base build.
#      - multy_camera_det_base_SHARE                    : The configuration files of this multy_camera_det_base.
#
#  The multy_camera_det_base CMake configuration file only was used in SCFI user repo.
# ===================================================================================


####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was Config.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

include(CMakeFindDependencyMacro)

foreach(dependency MANUALLY_ADDED_DEPENDENCIES-NOTFOUND)
    if(${dependency} STREQUAL "MANUALLY_ADDED_DEPENDENCIES-NOTFOUND")
        continue()
     endif()
    message(STATUS "Finding multy_camera_det_base dependency: ${dependency}")
    find_package(${dependency} PATHS ${PACKAGE_PREFIX_DIR}/lib/cmake/${dependency})
endforeach()

foreach(dependency  multy_camera_det_base_scfi_intf;-Wl,--as-needed)
    if(${dependency} STREQUAL "INTERFACE_LINK_LIBRARIES-NOTFOUND")
        continue()
    endif()
    if(${dependency} MATCHES "(.*)::(.*)")
        string(REGEX REPLACE "(.*)::(.*)" "\\2" dependency ${dependency})
        string(REPLACE "_" "-" dependency ${dependency})
        message(STATUS "Finding multy_camera_det_base dependency: ${dependency}")
        find_package(${dependency})
    endif()
endforeach()

foreach(dependency  SCFI_DEP-NOTFOUND)
    if(${dependency} STREQUAL "SCFI_DEP-NOTFOUND")
        continue()
    endif()
    message(STATUS "Finding multy_camera_det_base dependency: ${dependency}")
    find_package(${dependency} QUIET CMAKE_FIND_ROOT_PATH_BOTH)
    if(NOT ${dependency}_FOUND)
        set(multy_camera_det_base_FOUND False)
        message(WARNING "Finding multy_camera_det_base dependency ${dependency} fail.")
    endif()
endforeach()

foreach(dependency  ara-log;ara-per;ara-com;rm)
    if(${dependency} STREQUAL "MDC_SDK_DEP-NOTFOUND")
        continue()
    endif()
    if(${dependency} STREQUAL "hisdk9")
        set(ENV{HISDK2_SDK_PATH} ${SCFI_MDC_SDK})
        set(ENV{HISDK9_SDK_PATH} ${SCFI_MDC_SDK})
        set(ENV{ACL_SDK_PATH} ${SCFI_MDC_SDK}/local/Ascend/acllib/)
    elseif(${dependency} STREQUAL "hisdk2")
        set(ENV{HISDK2_SDK_PATH} ${SCFI_MDC_SDK})
        set(ENV{HISDK9_SDK_PATH} ${SCFI_MDC_SDK})
    elseif(${dependency} STREQUAL "acl")
        set(acl_DIR ${SCFI_MDC_SDK}/local/Ascend/acllib/cmake)
    endif()
    message(STATUS "Finding multy_camera_det_base dependency: ${dependency}")
    find_package(${dependency} QUIET CMAKE_FIND_ROOT_PATH_BOTH)
    if(NOT ${dependency}_FOUND)
        set(multy_camera_det_base_FOUND False)
        message(WARNING "Finding multy_camera_det_base dependency ${dependency} fail.")
    endif()
endforeach()

foreach(dep  RPM_DEP-NOTFOUND)
    if(${dep} STREQUAL "RPM_DEP-NOTFOUND")
        set(RPM_DEP-NOTFOUND "OFF")
    endif()
endforeach()
if(NOT DEFINED RPM_DEP-NOTFOUND)
    foreach(dependency  )
        message(STATUS "Including link directories: ${SCFI_MDC_SDK}/..${dependency}")
        link_directories(${SCFI_MDC_SDK}/../${dependency})
    endforeach()
endif()

include("${CMAKE_CURRENT_LIST_DIR}/multy_camera_det_base-targets.cmake")

set(multy_camera_det_base_LIBRARIES multy_camera_det_base)
set(multy_camera_det_base_INCLUDE_DIRS ${PACKAGE_PREFIX_DIR}/include/multy_camera_det_base)
set(multy_camera_det_base_SHARE ${PACKAGE_PREFIX_DIR}/share/multy_camera_det_base)
