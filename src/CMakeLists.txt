add_library(lab6_lib
        adapters/console_adapter.cpp

        config/config_loader.cpp
)

target_include_directories(lab6_lib
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_options(lab6_lib PRIVATE ${PROJECT_COMPILE_OPTIONS})
target_link_options(lab6_lib PRIVATE ${PROJECT_LINK_OPTIONS})

target_compile_definitions(lab6_lib PRIVATE
        RESOURCES_PATH="${CMAKE_SOURCE_DIR}/resources"
)