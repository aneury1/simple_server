#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <vector>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <regex>
#include <sstream>
// Define a structure to store client information
struct ClientInfo {
	SOCKET sockfd;
	sockaddr_in clientAddr;
};

struct Request {
	std::string url;
	std::map<std::string, std::string> parameters;
};
struct Response {
	std::unordered_map<std::string, std::string> headers;
	std::string body;
};

typedef Response* (*ParserEndpoint)(Request* request);

extern std::map<std::string, ParserEndpoint> registered_endpoint;
