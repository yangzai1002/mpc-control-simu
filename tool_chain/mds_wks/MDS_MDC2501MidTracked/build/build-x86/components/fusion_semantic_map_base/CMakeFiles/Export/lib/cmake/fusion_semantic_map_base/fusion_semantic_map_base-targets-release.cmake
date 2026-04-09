#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "fusion_semantic_map_base" for configuration "Release"
set_property(TARGET fusion_semantic_map_base APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(fusion_semantic_map_base PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libfusion_semantic_map_base.so.1.0.0"
  IMPORTED_SONAME_RELEASE "libfusion_semantic_map_base.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS fusion_semantic_map_base )
list(APPEND _IMPORT_CHECK_FILES_FOR_fusion_semantic_map_base "${_IMPORT_PREFIX}/lib/libfusion_semantic_map_base.so.1.0.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
