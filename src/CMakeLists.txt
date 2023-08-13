cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

find_package(spdlog 1.12.0 REQUIRED)
find_package(fmt 10.0 REQUIRED)

include_directories("${spdlog_INCLUDE_DIRS}")
include_directories("${fmt_INCLUDE_DIRS}")

set(SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/command_parser.cpp")

add_library(command_parser ${SOURCES})

target_compile_features(command_parser
	PRIVATE
	cxx_std_17
)

target_include_directories(command_parser
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../inc"
)

target_link_libraries(command_parser
    spdlog::spdlog
    fmt::fmt
)