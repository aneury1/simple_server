#pragma once
#include "HttpServer.h"

ParserEndpoint parseRequest(Request &request, ParserEndpoint notFoundHandler);
std::unordered_map<std::string, std::string> parseRequest(const std::string &request);
std::unordered_map<std::string, std::string> parseHeaders(const std::string &request);
void handleClient(ClientInfo *client);
std::string fetchJsBuilder(
    std::string url, 
    std::string body, 
    std::map<std::string,std::string>fetchOptions,
    std::map<std::string,std::string>httpHeaders);
Response *DefaultEndpoint(Request *request);
Response *DefaultEndpointW200(Request *request);

std::pair<std::string, ParserEndpoint> getEndpointFromMap(const std::string uri,const std::map<std::string, ParserEndpoint>& enpointList);
UrlDatatype getNodeDataTypeByPattern(const std::string& routeNode);
const std::string UrlDatatypeAsStr(const UrlDatatype& type);
UrlDatatype getDataTypeByValue(std::string value);
std::vector<std::string> splitWords(std::string word, char sp);
bool validatePatterByUrl(std::vector<std::string> uri, std::vector<std::string> pattern);
