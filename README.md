# AHTTPServer

## about
this project is a HTTP Server application that let people create endpoints, firstly designed to 
help me understand how socket, http, parsers and other stuff works;

## refactoring
by now I only have been playing around with std::thread, socket (without any library ), I know this approach is like reivent the wheel because I could use some library (like boost::asio,...) but in first instance I started from the bottom because I want to understand and give the opportunity to read how some thing works under the hood, obviously I would love to bring to people to use therefore 
I decided to create a framework where developer could use to create application without worry something would goes wrong and in addition they could add capabilty to their applications using this framework. in the next month I would add few component to this framework in order to achieve the thing i already mentioned.

## Build project
in order to build the project I set premake file that would generate the build script for the supported platform,later I would update this in order to give more information about the compiler flags for your desire platform.


## TODO 
There are a lot  of thing to do in this project here are the most need 
- Build Script Configuration
- Proxies 
- Virtual Proxies
- General Hash Functions
- Common Datastructure that could Support MT, NIO
- HTMLBuilder
- JSON,XML, yaml parser, other common parser.... 
- websocket
- SocketIO
- Connect to Other non http Server(Raw Server, UDP Server)
- change thread model to NIO/Select/Poll when configure.
- ...