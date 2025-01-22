#ifndef SIMPLE_SERVER_HTTP_CONNECTION_DEFINED
#define SIMPLE_SERVER_HTTP_CONNECTION_DEFINED
#pragma once
#include <functional>
#include <string>
#include <unordered_map>
#include "simple_server/base/simple_server_definitions.h"
#include "simple_server/http/simple_server_http_constant.h"
#include "simple_server/websocket/simple_server_websocket_connection.h"

namespace simple_server {
    

class Request
{
    public:
    request_verb requestVerb;
    std::string url;
    std::unordered_map<std::string, std::string> headers;
    std::unordered_map<std::string, std::string> query_params;
    std::string body;
};

class Response
{
    public:
    std::unordered_map<std::string, std::string> headers;
    std::string body;
    http_response_code status_code;
    virtual std::string buildResponse();
    Response();
protected:
    std::string raw_response;
};

typedef std::function<Response*(Request *)> endpointcb;

std::string content_type_to_string(http_content_type type);
std::string extract_http_verb(const std::string request);
request_verb parseRequestVerb(std::string buffer);
std::string http_response_code_to_string(http_response_code code);
std::string http_content_type_to_string(content_type type);
content_type http_content_type_from_string(const std::string& type);
std::string http_header_to_string(http_header header);
http_header string_to_http_header(const std::string& headerStr); 

}  // namespace  simple_server




#endif