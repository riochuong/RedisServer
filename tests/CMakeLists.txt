cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

set(CMAKE_VERBOSE_MAKEFILE ON)

find_package(spdlog 1.12.0 REQUIRED)
find_package(fmt 10.0 REQUIRED)
find_package(Catch2 3.4.0 REQUIRED)

include_directories("${spdlog_INCLUDE_DIRS}")
include_directories("${fmt_INCLUDE_DIRS}")
include_directories("${Catch2_INCLUDE_DIRS}")

set(SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test_command_parser.cpp")

add_executable(test_command_parser ${SOURCES})

#target_include_directories(test_command_parser
#    PRIVATE
#    ../inc
#)

target_compile_features(test_command_parser
	PRIVATE
	cxx_std_17
)

target_link_libraries(test_command_parser 
        "${Catch2_INCLUDE_DIRS}/../lib/libCatch2Main.a"
        "${Catch2_INCLUDE_DIRS}/../lib/libCatch2.a"
        command_parser
        spdlog::spdlog
        "${fmt_INCLUDE_DIRS_RELEASE}/../lib/libfmt.a"
)

message("catch2 Libraries ${Catch2_LIBRARIES}")
message("catch2 Libraries ${Catch2_INCLUDE_DIRS}")


add_test(test_resp_protocol test_command_parser)