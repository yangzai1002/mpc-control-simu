#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "driver_dev_null" for configuration "Release"
set_property(TARGET driver_dev_null APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(driver_dev_null PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libdriver_dev_null.so.1.0.0"
  IMPORTED_SONAME_RELEASE "libdriver_dev_null.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS driver_dev_null )
list(APPEND _IMPORT_CHECK_FILES_FOR_driver_dev_null "${_IMPORT_PREFIX}/lib/libdriver_dev_null.so.1.0.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
