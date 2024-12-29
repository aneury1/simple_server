cmake_minimum_required(VERSION 3.14)
project(simple_email_server LANGUAGES CXX)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Enable FetchContent module
include(FetchContent)

# Fetch Boost Library
FetchContent_Declare(
    Boost
    GIT_REPOSITORY https://github.com/boostorg/boost.git
    GIT_TAG boost-1.82.0 # Specify the version you want
    GIT_SHALLOW TRUE
)

# Ensure Boost is downloaded and built
FetchContent_MakeAvailable(Boost)

# Find OpenSSL
find_package(OpenSSL REQUIRED)

# Add Source File
add_executable(${PROJECT_NAME} smtp_server.cpp)

# Include Directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${Boost_INCLUDE_DIRS} 
    ${OPENSSL_INCLUDE_DIR}
)

# Link Libraries
target_link_libraries(${PROJECT_NAME} PRIVATE 
    Boost::system 
    Boost::thread 
    OpenSSL::SSL 
    OpenSSL::Crypto 
    pthread
)

# Enable SSL Support if OpenSSL is found
if (OpenSSL_FOUND)
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DSSL_ENABLED)
    message(STATUS "OpenSSL found, SSL support enabled.")
else()
    message(WARNING "OpenSSL not found, SSL support disabled.")
endif()

# Compiler Flags (Optional)
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()
