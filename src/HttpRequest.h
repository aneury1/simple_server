#pragma once
#include <vector>
#include <string>

using std::string;
#include "SocketServerBase.h"
#include "Constant.h"
#include "HttpHeader.h"
#include "HttpPath.h"

struct HttpRequest
{
	std::vector<HttpHeader> headers;
	string path;
	std::string raw;
	HTTP_VERB verb;
	HTTPPath* http_path;
	string body;
	SocketServer::SocketFd client_connection;
	HttpRequest(const char* str,std::string raw, SocketServer::SocketFd client_connection);
	
};

