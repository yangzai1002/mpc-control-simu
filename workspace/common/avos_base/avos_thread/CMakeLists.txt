cmake_minimum_required(VERSION 3.5)
project(avos_thread)

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

# # add_definitions(-w)

include_directories(
  ${PROJECT_SOURCE_DIR}/..
)

add_library(avos_thread SHARED
  avos_thread.cpp
  avos_thread_cell.cpp
  avos_thread_pool.cpp
  avos_thread_lock.cpp
  avos_thread_rwlock.cpp
  rwlock/rwlock_impl.cpp
  rwlock/rwlock_data.cpp
  rwlock/process_semaphore.cpp
  rwlock/thread_semaphore.cpp
)

target_link_libraries(avos_thread
  dl
  rt
  pthread
  avos_shared_memory
)

install(TARGETS avos_thread
  LIBRARY DESTINATION lib  # 动态库安装路径
  ARCHIVE DESTINATION lib  # 静态库安装路径
  RUNTIME DESTINATION bin  # 可执行文件安装路径
)

install(
  FILES
  thread_consts.h
  avos_thread_name.h
  avos_thread.h
  avos_thread_cell.h
  avos_thread_pool.h
  avos_thread_lock.h
  avos_thread_rwlock.h
  DESTINATION include/common_base/avos_thread 
)
