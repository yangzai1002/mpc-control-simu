#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "lidar_terrain_map_detection" for configuration "Release"
set_property(TARGET lidar_terrain_map_detection APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(lidar_terrain_map_detection PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/liblidar_terrain_map_detection.so.1.0.0"
  IMPORTED_SONAME_RELEASE "liblidar_terrain_map_detection.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS lidar_terrain_map_detection )
list(APPEND _IMPORT_CHECK_FILES_FOR_lidar_terrain_map_detection "${_IMPORT_PREFIX}/lib/liblidar_terrain_map_detection.so.1.0.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
