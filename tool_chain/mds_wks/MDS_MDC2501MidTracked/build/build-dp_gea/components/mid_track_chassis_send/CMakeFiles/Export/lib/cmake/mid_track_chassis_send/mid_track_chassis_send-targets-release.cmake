#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "mid_track_chassis_send" for configuration "Release"
set_property(TARGET mid_track_chassis_send APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(mid_track_chassis_send PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libmid_track_chassis_send.so.1.0.0"
  IMPORTED_SONAME_RELEASE "libmid_track_chassis_send.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS mid_track_chassis_send )
list(APPEND _IMPORT_CHECK_FILES_FOR_mid_track_chassis_send "${_IMPORT_PREFIX}/lib/libmid_track_chassis_send.so.1.0.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
