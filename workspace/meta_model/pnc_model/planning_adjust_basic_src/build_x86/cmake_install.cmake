# Install script for directory: /home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/install_x86")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "RelWithDebInfo")
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
  if(EXISTS "$ENV{DESTDIR}/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/../../../third_party/planning_adjust_basic/lib/libplanning_adjust_basic.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/../../../third_party/planning_adjust_basic/lib/libplanning_adjust_basic.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/../../../third_party/planning_adjust_basic/lib/libplanning_adjust_basic.so"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/../../../third_party/planning_adjust_basic/lib/libplanning_adjust_basic.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/../../../third_party/planning_adjust_basic/lib" TYPE SHARED_LIBRARY FILES "/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/build_x86/libplanning_adjust_basic.so")
  if(EXISTS "$ENV{DESTDIR}/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/../../../third_party/planning_adjust_basic/lib/libplanning_adjust_basic.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/../../../third_party/planning_adjust_basic/lib/libplanning_adjust_basic.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/../../../third_party/planning_adjust_basic/lib/libplanning_adjust_basic.so"
         OLD_RPATH "/home/idriver/ap_adsfi/workspace/third_party_x86/MDC_Ubuntu_X86/mdc_platform/lib:/home/idriver/ap_adsfi/workspace/third_party_x86/osqp-v0.6.0/lib:/home/idriver/ap_adsfi/workspace/third_party_x86/pcl-1.11.1/lib:/home/idriver/ap_adsfi/workspace/third_party_x86/osqp-eigen-v0.6.2/lib:/home/idriver/ap_adsfi/workspace/third_party_x86/casadi-3.6.7/lib:/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/../../../third_party/glog-0.6.0/lib:/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/../../../third_party/casadi-3.6.7/lib:/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/../../../third_party/proj4/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/../../../third_party/planning_adjust_basic/lib/libplanning_adjust_basic.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/../../../third_party/planning_adjust_basic/include/BlockPerceptionData/include/BlockPerceptionData.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/../../../third_party/planning_adjust_basic/include/BlockPerceptionData/include" TYPE FILE FILES "/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/src/BlockPerceptionData/include/BlockPerceptionData.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/../../../third_party/planning_adjust_basic/include/XBridge/include/XBridge.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/../../../third_party/planning_adjust_basic/include/XBridge/include" TYPE FILE FILES "/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/src/XBridge/include/XBridge.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/../../../third_party/planning_adjust_basic/include/XBridgeLevelingAndHeading/include/XBridgeLevelingAndHeading.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/../../../third_party/planning_adjust_basic/include/XBridgeLevelingAndHeading/include" TYPE FILE FILES "/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/src/XBridgeLevelingAndHeading/include/XBridgeLevelingAndHeading.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/../../../third_party/planning_adjust_basic/include/XBusinessFollow/include/XBusinessFollow.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/../../../third_party/planning_adjust_basic/include/XBusinessFollow/include" TYPE FILE FILES "/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/src/XBusinessFollow/include/XBusinessFollow.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/../../../third_party/planning_adjust_basic/include/XLevelingAndHeadingAdjustment/include/XLevelingAndHeadingAdjustment.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/../../../third_party/planning_adjust_basic/include/XLevelingAndHeadingAdjustment/include" TYPE FILE FILES "/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/src/XLevelingAndHeadingAdjustment/include/XLevelingAndHeadingAdjustment.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/../../../third_party/planning_adjust_basic/include/XLevelingAndHeadingAdjustment/src/HeadingAdjustment.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/../../../third_party/planning_adjust_basic/include/XLevelingAndHeadingAdjustment/src" TYPE FILE FILES "/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/src/XLevelingAndHeadingAdjustment/src/HeadingAdjustment.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/../../../third_party/planning_adjust_basic/include/XLevelingAndHeadingAdjustment/src/Leveling.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/../../../third_party/planning_adjust_basic/include/XLevelingAndHeadingAdjustment/src" TYPE FILE FILES "/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/src/XLevelingAndHeadingAdjustment/src/Leveling.h")
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/idriver/ap_adsfi/workspace/meta_model/pnc_model/planning_adjust_basic_src/build_x86/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
