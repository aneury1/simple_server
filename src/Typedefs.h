#pragma once
#include "HttpRequest.h"
#include <map>

typedef void (*handle_response_ptr)(int, char*, int);

typedef void (*http_request_handler)(HttpRequest request);

typedef  std::map<HttpIdentifier, http_request_handler> MiddleWareContainer;

typedef std::map<HttpIdentifier, http_request_handler> ConnectionHandlerContainer;