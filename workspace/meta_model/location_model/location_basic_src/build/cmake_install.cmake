# Install script for directory: /media/sf_zz/tmp/ap_adsfi/workspace/meta_model/location_model/location_basic_src

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
  if(EXISTS "$ENV{DESTDIR}/media/sf_zz/tmp/ap_adsfi/workspace/meta_model/location_model/location_basic_src/../../../third_party/location_basic_src/lib/liblocation_basic_src.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/media/sf_zz/tmp/ap_adsfi/workspace/meta_model/location_model/location_basic_src/../../../third_party/location_basic_src/lib/liblocation_basic_src.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/media/sf_zz/tmp/ap_adsfi/workspace/meta_model/location_model/location_basic_src/../../../third_party/location_basic_src/lib/liblocation_basic_src.so"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/media/sf_zz/tmp/ap_adsfi/workspace/meta_model/location_model/location_basic_src/../../../third_party/location_basic_src/lib/liblocation_basic_src.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/media/sf_zz/tmp/ap_adsfi/workspace/meta_model/location_model/location_basic_src/../../../third_party/location_basic_src/lib" TYPE SHARED_LIBRARY FILES "/media/sf_zz/tmp/ap_adsfi/workspace/meta_model/location_model/location_basic_src/build/liblocation_basic_src.so")
  if(EXISTS "$ENV{DESTDIR}/media/sf_zz/tmp/ap_adsfi/workspace/meta_model/location_model/location_basic_src/../../../third_party/location_basic_src/lib/liblocation_basic_src.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/media/sf_zz/tmp/ap_adsfi/workspace/meta_model/location_model/location_basic_src/../../../third_party/location_basic_src/lib/liblocation_basic_src.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/media/sf_zz/tmp/ap_adsfi/workspace/meta_model/location_model/location_basic_src/../../../third_party/location_basic_src/lib/liblocation_basic_src.so"
         OLD_RPATH "/usr/local/mdc_sdk_llvm/dp_gea/mdc_cross_compiler/sysroot/usr/lib/mdc_vector:/usr/local/mdc_sdk_llvm/dp_gea/mdc_cross_compiler/sysroot/usr/lib/mdc/base-plat:/media/sf_zz/tmp/ap_adsfi/workspace/meta_model/location_model/location_basic_src/../../../third_party/glog-0.6.0/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/media/sf_zz/tmp/ap_adsfi/workspace/meta_model/location_model/location_basic_src/../../../third_party/location_basic_src/lib/liblocation_basic_src.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/media/sf_zz/tmp/ap_adsfi/workspace/meta_model/location_model/location_basic_src/../../../third_party/location_basic_src/include/src/include/location_.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/media/sf_zz/tmp/ap_adsfi/workspace/meta_model/location_model/location_basic_src/../../../third_party/location_basic_src/include/src/include" TYPE FILE FILES "/media/sf_zz/tmp/ap_adsfi/workspace/meta_model/location_model/location_basic_src/src/include/location_.h")
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/media/sf_zz/tmp/ap_adsfi/workspace/meta_model/location_model/location_basic_src/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
