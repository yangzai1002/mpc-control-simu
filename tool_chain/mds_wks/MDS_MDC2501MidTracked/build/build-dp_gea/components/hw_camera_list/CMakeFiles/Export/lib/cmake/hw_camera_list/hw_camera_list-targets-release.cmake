#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "hw_camera_list" for configuration "Release"
set_property(TARGET hw_camera_list APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(hw_camera_list PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libhw_camera_list.so.1.0.0"
  IMPORTED_SONAME_RELEASE "libhw_camera_list.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS hw_camera_list )
list(APPEND _IMPORT_CHECK_FILES_FOR_hw_camera_list "${_IMPORT_PREFIX}/lib/libhw_camera_list.so.1.0.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
