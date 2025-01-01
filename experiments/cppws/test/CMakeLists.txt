# Fetch Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

# Add test executable
add_executable(my_tests base64test.cpp ../base64.cpp ../b64encode.cpp ../sha1.cpp)

# Link Google Test libraries
target_link_libraries(my_tests gtest_main gtest pthread)

# Register tests with CTest
add_test(NAME MyTests COMMAND my_tests)