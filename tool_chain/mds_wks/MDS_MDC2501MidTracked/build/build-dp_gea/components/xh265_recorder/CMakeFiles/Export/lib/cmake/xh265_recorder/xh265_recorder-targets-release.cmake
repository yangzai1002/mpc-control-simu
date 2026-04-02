#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "xh265_recorder" for configuration "Release"
set_property(TARGET xh265_recorder APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(xh265_recorder PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libxh265_recorder.so.1.0.0"
  IMPORTED_SONAME_RELEASE "libxh265_recorder.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS xh265_recorder )
list(APPEND _IMPORT_CHECK_FILES_FOR_xh265_recorder "${_IMPORT_PREFIX}/lib/libxh265_recorder.so.1.0.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
