#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "hw_fr671_radar" for configuration "Release"
set_property(TARGET hw_fr671_radar APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(hw_fr671_radar PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libhw_fr671_radar.so.1.0.0"
  IMPORTED_SONAME_RELEASE "libhw_fr671_radar.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS hw_fr671_radar )
list(APPEND _IMPORT_CHECK_FILES_FOR_hw_fr671_radar "${_IMPORT_PREFIX}/lib/libhw_fr671_radar.so.1.0.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
