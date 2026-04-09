# Install script for directory: /home/cxy/control-simulation/ap_auto/tool_chain/mds_wks/MDS_MDC2501MidTracked/components/Control

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/Control" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/Control")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/Control"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/home/cxy/control-simulation/ap_auto/tool_chain/mds_wks/MDS_MDC2501MidTracked/build/build-x86/components/Control/Control")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/Control" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/Control")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/Control"
         OLD_RPATH "/home/cxy/MDC_Ubuntu_X86/x86/lib/mdc/base-plat:/home/cxy/MDC_Ubuntu_X86/x86/lib64/cmake/ara-log/../..:/home/cxy/MDC_Ubuntu_X86/x86/lib64/cmake/ara-core/../../../lib64:/home/cxy/MDC_Ubuntu_X86/x86/lib64:/home/cxy/MDC_Ubuntu_X86/x86/lib64/cmake/ara-core/../../../lib:/home/cxy/MDC_Ubuntu_X86/x86/lib/cmake/ploglib/../../../lib64:/home/cxy/MDC_Ubuntu_X86/x86/lib64/cmake/ara-com/../../../lib64:/home/cxy/MDC_Ubuntu_X86/x86/lib64/cmake/ara-com/../../../lib:/home/idriver/ap_auto/workspace/third_party_x86/MDC_Ubuntu_X86/mdc_platform/lib:/home/idriver/ap_auto/workspace/third_party_x86/osqp-v0.6.0/lib:/home/idriver/ap_auto/workspace/third_party_x86/pcl-1.11.1/lib:/home/idriver/ap_auto/workspace/third_party_x86/opencv-4.5.5/lib:/home/idriver/ap_auto/workspace/third_party_x86/osqp-eigen-v0.6.2/lib:/home/idriver/ap_auto/workspace/third_party_x86/casadi-3.6.7/lib:/home/cxy/control-simulation/ap_auto/workspace/cmake/../third_party/glog-0.6.0/lib:/home/cxy/control-simulation/ap_auto/workspace/cmake/../third_party/proj4/lib:/home/cxy/control-simulation/ap_auto/workspace/cmake/../third_party/fmt/lib:/home/cxy/control-simulation/ap_auto/workspace/cmake/../third_party/freetype/lib:/home/cxy/control-simulation/ap_auto/workspace/meta_model/pnc_model/control/../../../third_party/control_basic/lib:/home/cxy/control-simulation/ap_auto/workspace/meta_model/pnc_model/control/../../../third_party/proj4/lib:/home/cxy/control-simulation/ap_auto/tool_chain/mds_wks/MDS_MDC2501MidTracked/build/build-x86/components/control_base:/home/cxy/MDC_Ubuntu_X86/x86/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/Control")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/conf" TYPE DIRECTORY OPTIONAL FILES "/home/cxy/control-simulation/ap_auto/tool_chain/mds_wks/MDS_MDC2501MidTracked/components/Control/conf/" FILES_MATCHING REGEX "/[^/]*$")
endif()

