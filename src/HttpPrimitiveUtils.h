#pragma once
#include "HttpServer.h"

ParserEndpoint parseRequest(Request &request, ParserEndpoint notFoundHandler);
std::unordered_map<std::string, std::string> parseRequest(const std::string &request);
std::unordered_map<std::string, std::string> parseHeaders(const std::string &request);
void handleClient(ClientInfo *client);
