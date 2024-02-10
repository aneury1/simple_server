# BUILD_TESTING variable is created by include(CTest)
# It is set to ON by default
if (BUILD_TESTING)
    add_executable(my_test my_test.cpp)
    add_executable(test_http_server testHttpServer.cpp)
    target_link_libraries(my_test PRIVATE GTest::gtest_main)
    target_link_libraries(test_http_server PRIVATE GTest::gtest_main)

    include(GoogleTest)
    # Finds all the Google tests associated with the executable
    gtest_discover_tests(my_test)
    gtest_discover_tests(test_http_server)
endif()