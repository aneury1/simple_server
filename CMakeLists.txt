cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME simple_server)
project(${PROJECT_NAME} LANGUAGES CXX)


# Build opts.
option(ENABLE_CLANG_TIDY "Enable clang-tidy" FALSE)
option(ENABLE_COVERAGE "Enable code coverage" FALSE)
option(ENABLE_TESTING "Enable code Testing" TRUE)
option(ENABLE_THIRDPARTY "Allow Insta" FALSE)


if(ENABLE_CLANG_TIDY)
    set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=*")
  # set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=modernize-*,performance-*"
endif()

if (ENABLE_COVERAGE)
    message(STATUS "Code Coverage enabled")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -O0 -g")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

if(ENABLE_THIRDPARTY)
    include(FetchContent)
    FetchContent_Declare(
        nlohmannjson
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2
    )
    FetchContent_MakeAvailable(nlohmannjson)
endif()

# Link jsonparser to your library
file(GLOB_RECURSE SRCS src/*.cpp)




include_directories(includes PUBLIC)
include_directories(third_party/jzmn PUBLIC)

message(STATUS "Building ${PROJECT_NAME}...")

# add_library(${PROJECT_NAME} ${SRCS})
add_executable(${PROJECT_NAME} ${SRCS})

if(ENABLE_THIRDPARTY)
    target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
endif()


list(APPEND IncludesDirs 
     PRIVATE includes)

target_include_directories(${PROJECT_NAME} BEFORE ${IncludesDirs})

# list(APPEND LibrariesList TestLibrary)
# target_link_libraries(${PROJECT_NAME} PRIVATE ${LibrariesList})

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/includes>
    $<INSTALL_INTERFACE:includes>
)

if(ENABLE_INSTALL)
 install(TARGETS simple_http_server DESTINATION lib)
endif()


if(ENABLE_TESTING)
    message("Testing is enabled")
    enable_testing()
    include(CTest)
    #add_subdirectory(google_test) # Google could be fetched.
    add_subdirectory(tests)
    
    #add_subdirectory(sample)
    #include(FetchContent)
    # FetchContent_Declare(
    #   googletest
    #   URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    #)
    # # For Windows: Prevent overriding the parent project's compiler/linker settings
    #set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    #FetchContent_MakeAvailable(googletest)
    #enable_testing()
    
    
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

endif()