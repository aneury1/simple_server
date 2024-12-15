cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME simple_http_server)

set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=*")
# set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=modernize-*,performance-*"

# Enable Code Coverage
option(ENABLE_COVERAGE "Enable code coverage" ON)

if (ENABLE_COVERAGE)
    message(STATUS "Code Coverage enabled")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -O0 -g")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()


project(${PROJECT_NAME} LANGUAGES CXX)

file(GLOB_RECURSE SRCS src/*.cpp)

include_directories(includes PUBLIC)

message(STATUS "Building ${PROJECT_NAME}...")

add_library(simple_http_server ${SRCS})

list(APPEND IncludesDirs 
     PRIVATE includes)

target_include_directories(${PROJECT_NAME} BEFORE ${IncludesDirs})

# list(APPEND LibrariesList TestLibrary)
# target_link_libraries(${PROJECT_NAME} PRIVATE ${LibrariesList})

install(TARGETS simple_http_server DESTINATION lib)
include(CTest)
add_subdirectory(external)
add_subdirectory(tests)
#add_subdirectory(sample)

# include(FetchContent)
# FetchContent_Declare(
#   googletest
#   URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
# )
# # For Windows: Prevent overriding the parent project's compiler/linker settings
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# FetchContent_MakeAvailable(googletest)


# enable_testing()

# add_executable(
#   hello_test
#   tests/testmain.cc
# )
# target_link_libraries(
#   hello_test
#   GTest::gtest_main
# )

# include(GoogleTest)
# gtest_discover_tests(hello_test)


