cmake_minimum_required(VERSION 3.22)

project(foundation-json-test DESCRIPTION "Foundation is my own \"standard library\" for C++.")

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wno-vla-extension -std=c++20")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(FOUNDATION_IO_FS ON)

add_compile_definitions(FOUNDATION_IO_FS)

add_executable(foundation-json-test
	src/main.cpp)

add_subdirectory(foundation)

target_link_libraries(foundation-json-test foundation)

set(ARCH_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}/${CMAKE_SYSTEM_PROCESSOR}")

set_target_properties(foundation-json-test
	PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${ARCH_OUTPUT_DIR}/lib"
	LIBRARY_OUTPUT_DIRECTORY "${ARCH_OUTPUT_DIR}/lib"
	RUNTIME_OUTPUT_DIRECTORY "${ARCH_OUTPUT_DIR}/bin")

target_compile_features(foundation PRIVATE cxx_std_20)
