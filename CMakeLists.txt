cmake_minimum_required(VERSION 3.2)

project(AHTTPServer)

add_executable( 
    AHTTPServer
    src/HTTPPath.cpp
    src/HttpRequest.cpp
    src/JSON.cpp
    src/Server.cpp
    src/main.cpp
)

 