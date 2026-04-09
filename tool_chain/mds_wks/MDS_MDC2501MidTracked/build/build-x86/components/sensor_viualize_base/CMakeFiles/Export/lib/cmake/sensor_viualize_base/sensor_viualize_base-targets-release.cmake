#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "sensor_viualize_base" for configuration "Release"
set_property(TARGET sensor_viualize_base APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(sensor_viualize_base PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libsensor_viualize_base.so.1.0.0"
  IMPORTED_SONAME_RELEASE "libsensor_viualize_base.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS sensor_viualize_base )
list(APPEND _IMPORT_CHECK_FILES_FOR_sensor_viualize_base "${_IMPORT_PREFIX}/lib/libsensor_viualize_base.so.1.0.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
