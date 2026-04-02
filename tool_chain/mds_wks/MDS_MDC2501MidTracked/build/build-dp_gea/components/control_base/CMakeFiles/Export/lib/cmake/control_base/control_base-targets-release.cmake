#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "control_base" for configuration "Release"
set_property(TARGET control_base APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(control_base PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libcontrol_base.so.1.0.0"
  IMPORTED_SONAME_RELEASE "libcontrol_base.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS control_base )
list(APPEND _IMPORT_CHECK_FILES_FOR_control_base "${_IMPORT_PREFIX}/lib/libcontrol_base.so.1.0.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
