#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>
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


enum class HttpResponseCode {
  // Informational codes (1xx)
  Continue = 100,
  SwitchingProtocols = 101,

  // Success codes (2xx)
  OK = 200,
  Created = 201,
  Accepted = 202,

  // Redirection codes (3xx)
  MovedPermanently = 301,
  Found = 302,
  SeeOther = 303,

  // Client error codes (4xx)
  BadRequest = 400,
  Unauthorized = 401,
  Forbidden = 403,
  NotFound = 404,

  // Server error codes (5xx)
  InternalServerError = 500,
  NotImplemented = 501,

  // You can add other relevant codes here
};

enum class HttpContentType {
    TEXT_HTML,
    TEXT_PLAIN,
    APPLICATION_JSON,
    APPLICATION_XML,
    MULTIPART_FORM_DATA,
    APPLICATION_OCTET_STREAM,
    IMAGE_PNG,
    IMAGE_JPEG,
    UNKNOWN
};
std::string ContentTypetoString(HttpContentType type);


enum class UrlDatatype{
    Integer,
    Str,
    Boolean,
    InvalidUrlDatatype
};

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
    ~ClientInfo();
};

struct Request
{
    RequestVerb requestVerb;
    std::string url;
    std::unordered_map<std::string, std::string> headers;
    std::unordered_map<std::string, std::string> queryParams;
    std::string body;
};
struct Response
{
    std::unordered_map<std::string, std::string> headers;
    std::string body;
    HttpResponseCode statusCode;

    virtual std::string buildResponse();


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

///typedef Response *(*ParserEndpoint)(Request *request);
typedef std::function<Response*(Request *)> ParserEndpoint;

extern std::map<std::string, ParserEndpoint> registered_endpoint;
extern std::map<std::string, ParserEndpoint> registered_post_endpoint;
