#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "hw_l106_lidar" for configuration "Release"
set_property(TARGET hw_l106_lidar APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(hw_l106_lidar PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libhw_l106_lidar.so.1.0.0"
  IMPORTED_SONAME_RELEASE "libhw_l106_lidar.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS hw_l106_lidar )
list(APPEND _IMPORT_CHECK_FILES_FOR_hw_l106_lidar "${_IMPORT_PREFIX}/lib/libhw_l106_lidar.so.1.0.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
