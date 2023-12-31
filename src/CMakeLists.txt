cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

find_package(spdlog 1.12.0 REQUIRED)
find_package(fmt 10.0 REQUIRED)
find_package(asio 1.28.1 REQUIRED)

include_directories("${spdlog_INCLUDE_DIRS}")
include_directories("${fmt_INCLUDE_DIRS}")
include_directories("${asio_INCLUDE_DIRS}")

set(SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/command_parser.cpp")

add_library(command_parser "${CMAKE_CURRENT_SOURCE_DIR}/command_parser.cpp")
add_library(utils "${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp")
add_executable(redis_server "${CMAKE_CURRENT_SOURCE_DIR}/server.cpp")

target_compile_features(command_parser
	PRIVATE
	cxx_std_17
)

target_include_directories(command_parser
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../inc"
)

target_include_directories(utils
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../inc"
)

target_link_libraries(command_parser
    spdlog::spdlog
    utils
    "${fmt_INCLUDE_DIRS_RELEASE}/../lib/libfmt.a"
)

target_link_libraries(utils
    spdlog::spdlog
    "${fmt_INCLUDE_DIRS_RELEASE}/../lib/libfmt.a"
)

target_link_libraries(redis_server
    spdlog::spdlog
    command_parser
    utils
    asio::asio
    "${fmt_INCLUDE_DIRS_RELEASE}/../lib/libfmt.a"
)