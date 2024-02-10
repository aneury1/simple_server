#pragma once
#include <iostream>
#include <string>
#include <unordered_map>

#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#ifndef WIN32
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#define closesocket close
typedef int SOCKET;
#endif

///#include "HttpServer.h"
enum class RequestVerb{
    Get,Post
    
#ifdef API2_0
    ,Put,Delete,Options
 #endif
};
// Define a structure to store client information
struct ClientInfo
{
    SOCKET sockfd;
    sockaddr_in clientAddr;
};

struct Request
{
    RequestVerb requestVerb;
    std::string url;
    std::unordered_map<std::string, std::string> parameters;
    std::unordered_map<std::string, std::string> urlParams;
    std::string body;
};
struct Response
{
    std::unordered_map<std::string, std::string> headers;
    std::string body;
    int statusCode;

#ifdef API2_0
    Response createResponseForHtmlContent();
    Response createResponseForJs();
    Response createResponseForJson();
    Response createResponseTextPlain();
    std::string getResponse();
#endif 


private:
    std::string raw_response;
};

typedef Response *(*ParserEndpoint)(Request *request);

extern std::map<std::string, ParserEndpoint> registered_endpoint;
extern std::map<std::string, ParserEndpoint> registered_post_endpoint;
