#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "xh265_rtp_pusher" for configuration "Release"
set_property(TARGET xh265_rtp_pusher APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(xh265_rtp_pusher PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libxh265_rtp_pusher.so.1.0.0"
  IMPORTED_SONAME_RELEASE "libxh265_rtp_pusher.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS xh265_rtp_pusher )
list(APPEND _IMPORT_CHECK_FILES_FOR_xh265_rtp_pusher "${_IMPORT_PREFIX}/lib/libxh265_rtp_pusher.so.1.0.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
