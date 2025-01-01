cmake_minimum_required(VERSION 3.18)
project(APIService)

# Set parent directory path
set(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

# Add the library directory (assuming the library is defined in the parent)
add_subdirectory(${PARENT_DIR} ${CMAKE_CURRENT_BINARY_DIR}/build)

# Include the library headers
include_directories(${PARENT_DIR}/includes)

# Create the sample_project executable
add_executable(sample_project main.cpp)

# Include the parent directory headers (to find AHttpServer headers)
target_include_directories(sample_project PRIVATE ${PARENT_DIR}/includes)

# Link the library to sample_project
target_link_libraries(sample_project PRIVATE simple_server)

# Optionally, create APIService executable (if needed)
# Uncomment the following lines if you need a separate executable
# add_executable(APIService main.cpp)
# target_link_libraries(APIService PRIVATE AHttpServer)
