cmake_minimum_required(VERSION 3.10)

project(AHTTPServer LANGUAGES CXX)

file(GLOB_RECURSE SRCS src/*.cpp)

add_executable(${PROJECT_NAME} ${SRCS})

