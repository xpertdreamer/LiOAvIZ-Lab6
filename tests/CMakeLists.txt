find_package(GTest QUIET)

if(GTest_FOUND)
    message(STATUS "GoogleTest found, building tests")

    add_executable(test_backend test_backend.cpp)
    target_include_directories(test_backend PRIVATE ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(test_backend PRIVATE lab6_lib GTest::gtest GTest::gtest_main)
    target_compile_options(test_backend PRIVATE ${PROJECT_COMPILE_OPTIONS})
    add_test(NAME backend_tests COMMAND test_backend)

    add_executable(test_adapters test_adapters.cpp)
    target_include_directories(test_adapters PRIVATE ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(test_adapters PRIVATE lab6_lib GTest::gtest GTest::gtest_main)
    target_compile_options(test_adapters PRIVATE ${PROJECT_COMPILE_OPTIONS})
    add_test(NAME adapters_tests COMMAND test_adapters)

    add_executable(test_config test_config.cpp)
    target_include_directories(test_config PRIVATE ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(test_config PRIVATE lab6_lib GTest::gtest GTest::gtest_main)
    target_compile_options(test_config PRIVATE ${PROJECT_COMPILE_OPTIONS})
    add_test(NAME config_tests COMMAND test_config)

else()
    message(WARNING "GoogleTest not found, tests will not be built")
endif()