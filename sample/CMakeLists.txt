cmake_minimum_required(VERSION 3.18)
project(APIService)

# Include the parent directory to access the library
# add_subdirectory(../build)

set(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR}/build)

target_include_directories(${PROJECT_NAME} BEFORE ${PARENT_DIR})

# Create the executable
add_executable(sample_project main.cpp)

# Link the static library to the executable
target_link_libraries(sample_project PRIVATE AHttpServer)


add_executable(APIService main.cpp)

