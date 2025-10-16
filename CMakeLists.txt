cmake_minimum_required(VERSION 3.31)
project(LiOAvIZ_Lab6)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if (POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)
endif ()

set(SOURCES
        main.cpp
)

add_executable(LiOAvIZ_Lab6 ${SOURCES})

target_include_directories(LiOAvIZ_Lab6 PRIVATE

)

target_precompile_headers(LiOAvIZ_Lab6 PRIVATE

)

target_compile_options(LiOAvIZ_Lab6 PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic -Wshadow>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>

        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wnull-dereference -Wdouble-promotion>
)

target_compile_options(LiOAvIZ_Lab6 PRIVATE
        $<$<CONFIG:Debug>:
        $<$<CXX_COMPILER_ID:GNU,Clang>:-O0 -g3 -fno-omit-frame-pointer>
        $<$<CXX_COMPILER_ID:MSVC>:/Od /Zi /Ob0>
        >
)

target_compile_options(LiOAvIZ_Lab6 PRIVATE
        $<$<CONFIG:Release>:
        $<$<CXX_COMPILER_ID:GNU,Clang>:-O3 -flto -DNDEBUG -march=native>
        $<$<CXX_COMPILER_ID:MSVC>:/O2 /Ob2 /DNDEBUG>
        >
)

target_link_options(LiOAvIZ_Lab6 PRIVATE
        $<$<CONFIG:Release>:
        $<$<CXX_COMPILER_ID:GNU,Clang>:-flto>
        >
)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND NOT WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Enabling sanitizers for Clang")

    target_compile_options(LiOAvIZ_Lab6 PRIVATE
            -fsanitize=address
            -fsanitize=undefined
            -fsanitize-leak
            -fno-sanitize-recover=all
    )
    target_link_options(LiOAvIZ_Lab6 PRIVATE
            -fsanitize=address
            -fsanitize=undefined
            -fsanitize-leak
    )

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND NOT WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(STATUS "Enabling sanitizers for Clang")

        target_compile_options(LiOAvIZ_Lab6 PRIVATE
                -fsanitize=address
                -fsanitize=undefined
                -fsanitize-leak
                -fno-sanitize-recover=all
        )
        target_link_options(LiOAvIZ_Lab6 PRIVATE
                -fsanitize=address
                -fsanitize=undefined
                -fsanitize-leak
        )

    elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(STATUS "Sanitizers disabled - not supported with current configuration")
        message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}, Platform: ${CMAKE_SYSTEM_NAME}")
    endif()

    if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(LiOAvIZ_Lab6 PRIVATE
                -fstack-protector-strong
                -Wformat-security
                -Warray-bounds=2
        )
        message(STATUS "Enabled enhanced debug checks for GCC")
    endif()
endif ()

set_target_properties(LiOAvIZ_Lab6 PROPERTIES
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON
        OUTPUT_NAME "LiOAvIZ_Lab6"
)

message(STATUS "Project configured successfully: ${PROJECT_NAME}")
message(STATUS "Binary output: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")