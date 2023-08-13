cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)

set(CMAKE_TOOLCHAIN_FILE build/conan_toolchain.cmake)

project(redis-server)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

add_subdirectory(tests)
add_subdirectory(src)