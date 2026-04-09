# Install script for directory: /home/cxy/control-simulation/ap_auto/tool_chain/mds_wks/MDS_MDC2501MidTracked/components/fusion_semantic_map_base

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
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libfusion_semantic_map_base.so.1.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libfusion_semantic_map_base.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/home/cxy/control-simulation/ap_auto/tool_chain/mds_wks/MDS_MDC2501MidTracked/build/build-dp_gea/components/fusion_semantic_map_base/libfusion_semantic_map_base.so.1.0.0"
    "/home/cxy/control-simulation/ap_auto/tool_chain/mds_wks/MDS_MDC2501MidTracked/build/build-dp_gea/components/fusion_semantic_map_base/libfusion_semantic_map_base.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libfusion_semantic_map_base.so.1.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libfusion_semantic_map_base.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/usr/local/mdc_sdk_llvm/dp_gea/mdc_cross_compiler/sysroot/usr/lib/mdc/base-plat:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libfusion_semantic_map_base.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libfusion_semantic_map_base.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libfusion_semantic_map_base.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/cxy/control-simulation/ap_auto/tool_chain/mds_wks/MDS_MDC2501MidTracked/build/build-dp_gea/components/fusion_semantic_map_base/libfusion_semantic_map_base.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libfusion_semantic_map_base.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libfusion_semantic_map_base.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libfusion_semantic_map_base.so"
         OLD_RPATH "/usr/local/mdc_sdk_llvm/dp_gea/mdc_cross_compiler/sysroot/usr/lib/mdc/base-plat:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libfusion_semantic_map_base.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/fusion_semantic_map_base" TYPE DIRECTORY OPTIONAL FILES "/home/cxy/control-simulation/ap_auto/tool_chain/mds_wks/MDS_MDC2501MidTracked/components/fusion_semantic_map_base/include/public/" FILES_MATCHING REGEX "/[^/]*$")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE FILE OPTIONAL FILES "/home/cxy/zjy/MDC-Tools/MDC_Development_Studio-Ubuntu/MDC_SCFI/cmake/template/install.sh")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/fusion_semantic_map_base" TYPE DIRECTORY OPTIONAL FILES
    "/home/cxy/control-simulation/ap_auto/tool_chain/mds_wks/MDS_MDC2501MidTracked/components/fusion_semantic_map_base/manifest"
    "/home/cxy/control-simulation/ap_auto/tool_chain/mds_wks/MDS_MDC2501MidTracked/components/fusion_semantic_map_base/config"
    "/home/cxy/control-simulation/ap_auto/tool_chain/mds_wks/MDS_MDC2501MidTracked/components/fusion_semantic_map_base/outputcfg"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/fusion_semantic_map_base" TYPE FILE OPTIONAL FILES
    "/home/cxy/control-simulation/ap_auto/tool_chain/mds_wks/MDS_MDC2501MidTracked/components/fusion_semantic_map_base/component.xml"
    "/home/cxy/control-simulation/ap_auto/tool_chain/mds_wks/MDS_MDC2501MidTracked/components/fusion_semantic_map_base/changelog.txt"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/fusion_semantic_map_base/fusion_semantic_map_base-targets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/fusion_semantic_map_base/fusion_semantic_map_base-targets.cmake"
         "/home/cxy/control-simulation/ap_auto/tool_chain/mds_wks/MDS_MDC2501MidTracked/build/build-dp_gea/components/fusion_semantic_map_base/CMakeFiles/Export/lib/cmake/fusion_semantic_map_base/fusion_semantic_map_base-targets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/fusion_semantic_map_base/fusion_semantic_map_base-targets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/fusion_semantic_map_base/fusion_semantic_map_base-targets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/fusion_semantic_map_base" TYPE FILE FILES "/home/cxy/control-simulation/ap_auto/tool_chain/mds_wks/MDS_MDC2501MidTracked/build/build-dp_gea/components/fusion_semantic_map_base/CMakeFiles/Export/lib/cmake/fusion_semantic_map_base/fusion_semantic_map_base-targets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/fusion_semantic_map_base" TYPE FILE FILES "/home/cxy/control-simulation/ap_auto/tool_chain/mds_wks/MDS_MDC2501MidTracked/build/build-dp_gea/components/fusion_semantic_map_base/CMakeFiles/Export/lib/cmake/fusion_semantic_map_base/fusion_semantic_map_base-targets-release.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/fusion_semantic_map_base" TYPE FILE FILES "/home/cxy/control-simulation/ap_auto/tool_chain/mds_wks/MDS_MDC2501MidTracked/build/build-dp_gea/components/fusion_semantic_map_base/fusion_semantic_map_base-config.cmake")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/fusion_semantic_map_base" TYPE FILE FILES "/home/cxy/control-simulation/ap_auto/tool_chain/mds_wks/MDS_MDC2501MidTracked/build/build-dp_gea/components/fusion_semantic_map_base/fusion_semantic_map_base-config-version.cmake")
endif()

