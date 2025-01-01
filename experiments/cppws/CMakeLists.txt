cmake_minimum_required(VERSION 3.28)

project(cppws)

set(SRC cppws.cpp base64.cpp b64encode.cpp sha1.cpp)

add_executable(
    ${PROJECT_NAME}
    ${SRC}
)

# Add the test directory
enable_testing()
add_subdirectory(test)