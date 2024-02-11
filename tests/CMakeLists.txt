# BUILD_TESTING variable is created by include(CTest)
# It is set to ON by default

set(PROJECT_ROOT_PATH ${PROJECT_SOURCE_DIR})

message(STATUS "Project root Path ${PROJECT_ROOT_PATH}")

include_directories(${PROJECT_ROOT_PATH}/includes/)

if (BUILD_TESTING)
    add_executable(my_test my_test.cpp)
    add_executable(test_http_server testHttpServer.cpp)
    target_link_libraries(my_test PRIVATE GTest::gtest_main  AHttpServer)
    target_link_libraries(test_http_server PRIVATE GTest::gtest_main  AHttpServer)

    include(GoogleTest)
    # Finds all the Google tests associated with the executable
    gtest_discover_tests(my_test)
    gtest_discover_tests(test_http_server)
endif()