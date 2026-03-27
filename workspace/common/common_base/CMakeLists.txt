cmake_minimum_required(VERSION 3.5)
project(common_base)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic 
  -Wno-unused-parameter  -Wno-unused-variable -Wno-sign-compare -Wno-return-type -Wno-type-limits )
endif()


include(${KTD_ROOT}/share/cmake/idpware_board.cmake)

# add_definitions(-w)

include_directories(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/..
  ${PROJECT_SOURCE_DIR}/../math/include
)

add_library(common_base SHARED
  param_config_manager/param_config_manager.cpp

  util/timer.cpp
  util/util.cpp
  util/string_tokenizer.cpp
  util/clog.cpp
  util/geotool.cc

  uart/cuart.cpp
  spi/spi.cpp
  socket_can/socket_can.cpp

  socket_transmitter/client/socket_client_transmitter.cpp
  socket_transmitter/server/socket_server_transmitter.cpp
)

target_link_libraries(common_base
  rt 
  pthread 
  common_proto_proto
  dl 
  proj
  ${LIBS_LIST}
)

install(TARGETS common_base
  EXPORT common_base
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

file(GLOB PARAM_CONFIG_MANAGER_HEADER_FILES param_config_manager/*.h)
install(
  FILES 
  ${PARAM_CONFIG_MANAGER_HEADER_FILES}
  DESTINATION include/common_base/config
)
install(
  FILES 
  ${PARAM_CONFIG_MANAGER_HEADER_FILES}
  DESTINATION include/common_base/param_config_manager
)

file(GLOB UTIL_HEADER_FILES util/*.h)
install(
  FILES 
  ${UTIL_HEADER_FILES}
  DESTINATION include/common_base/util
)

file(GLOB SPI_HEADER_FILES spi/*.h)
install(
  FILES 
  ${SPI_HEADER_FILES}
  DESTINATION include/common_base/communication/spi_communication
)

file(GLOB SOCKET_SERVER_HEADER_FILES socket_server/*.h)
install(
  FILES 
  ${SOCKET_SERVER_HEADER_FILES}
  DESTINATION include/common_base/communication/socket_server
)

install(
  FILES 
  socket_can/socket_can.h
  DESTINATION include/common_base/communication/socket_can
)

install(
  FILES 
  uart/cuart.h
  DESTINATION include/common_base/communication/uart
)

install(
  FILES 
  socket_transmitter/socket_header.h
  DESTINATION include/common_base/communication/socket_transmitter
)

install(
  FILES 
  socket_transmitter/server/socket_server_transmitter.h
  DESTINATION include/common_base/communication/socket_transmitter/server
)

install(
  FILES 
  socket_transmitter/client/socket_client_transmitter.h
  DESTINATION include/common_base/communication/socket_transmitter/client
)

