set(PROJECT_COMPILE_OPTIONS "")
set(PROJECT_LINK_OPTIONS "")

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    list(APPEND PROJECT_COMPILE_OPTIONS
            -Wall
            -Wextra
            -Wpedantic
            -Wshadow
            -Wnull-dereference
            -Wdouble-promotion
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    list(APPEND PROJECT_COMPILE_OPTIONS
            /W4
            /WX
    )
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        list(APPEND PROJECT_COMPILE_OPTIONS
                -O0
                -g3
                -fno-omit-frame-pointer
        )
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        list(APPEND PROJECT_COMPILE_OPTIONS
                /Od
                /Zi
                /Ob0
        )
    endif()
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        list(APPEND PROJECT_COMPILE_OPTIONS
                -O3
                -flto
                -DNDEBUG
        )
        list(APPEND PROJECT_LINK_OPTIONS -flto)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        list(APPEND PROJECT_COMPILE_OPTIONS
                /O2
                /Ob2
                /DNDEBUG
        )
    endif()
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND NOT WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Enabling sanitizers for Clang")
    list(APPEND PROJECT_COMPILE_OPTIONS
            -fsanitize=address
            -fsanitize=undefined
            -fno-sanitize-recover=all
    )
    list(APPEND PROJECT_LINK_OPTIONS
            -fsanitize=address
            -fsanitize=undefined
    )
endif()

message(STATUS "Compile options: ${PROJECT_COMPILE_OPTIONS}")