cmake_minimum_required(VERSION 3.31)
project(LiOAvIZ_Lab6 VERSION 1.0.0 LANGUAGES CXX)

if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

option(BUILD_TESTS "Build tests" ON)

include(cmake/compiler_options.cmake)

add_subdirectory(src)

add_executable(LiOAvIZ_Lab6 src/main.cpp
        include/backend/matrix_gen.h)

target_link_libraries(LiOAvIZ_Lab6 PRIVATE lab6_lib)

target_include_directories(LiOAvIZ_Lab6 PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_custom_command(TARGET LiOAvIZ_Lab6 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources/
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/
        COMMENT "Copying resources to output directory"
)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

set_target_properties(LiOAvIZ_Lab6 PROPERTIES
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON
        OUTPUT_NAME "LiOAvIZ_Lab6"
)

message(STATUS "Project configured successfully: ${PROJECT_NAME}")
message(STATUS "Binary output: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")