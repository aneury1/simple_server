#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
 
#include <thread>
#include <vector>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <regex>
#include <sstream>

#ifndef WIN32
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#define closesocket close
typedef int SOCKET;
#endif


///#include "HttpServer.h"

// Define a structure to store client information
struct ClientInfo {
	SOCKET sockfd;
	sockaddr_in clientAddr;
};

struct Request {
	std::string url;
	std::unordered_map<std::string, std::string> parameters;
	std::unordered_map<std::string, std::string> urlParams;
};
struct Response {
	std::unordered_map<std::string, std::string> headers;
	std::string body;
	int statusCode;

    
	Response createResponseForHtmlContent();
	Response createResponseForJs();
	Response createResponseForJson();
	Response createResponseTextPlain();

    std::string getResponse();

    private:

	std::string raw_response;

};

typedef Response* (*ParserEndpoint)(Request* request);

extern std::map<std::string, ParserEndpoint> registered_endpoint;
