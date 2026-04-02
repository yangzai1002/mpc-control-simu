#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "bd21_imu" for configuration "Release"
set_property(TARGET bd21_imu APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(bd21_imu PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libbd21_imu.so.1.0.0"
  IMPORTED_SONAME_RELEASE "libbd21_imu.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS bd21_imu )
list(APPEND _IMPORT_CHECK_FILES_FOR_bd21_imu "${_IMPORT_PREFIX}/lib/libbd21_imu.so.1.0.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
