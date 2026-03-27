# toplevel CMakeLists.txt for a scfi workspace

cmake_minimum_required(VERSION 3.10.1)

project(scfi)

set(CMAKE_CXX_STANDARD 14)

find_package(scfi CMAKE_FIND_ROOT_PATH_BOTH)

if(NOT ${scfi_FOUND})
    message(FATAL_ERROR "find_package(scfi) failed. \
    scfi was not found in the CMAKE_PREFIX_PATH. One reason may be that no SCFI setup.sh was sourced before.")
endif()

scfi_env(SHOW)

scfi_load_components()