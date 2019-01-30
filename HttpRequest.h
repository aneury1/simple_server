#pragma once
#include <string>
using std::string;
#include <vector>
#include <iostream>
#include "HTTPPath.h"

enum class HTTP_VERB{HTTP_GET = 1, HTTP_POST, HTTP_PUT, HTTP_OPTIONS,HTTP_INVALID_VERB};



struct HttpHeader{
    string key;
    string value;
    HttpHeader(string bulk);
    HttpHeader(string key, string value);
    string toString();
};

struct HTTPRequest
{
   std::vector<HttpHeader> headers;
   string path;
   HTTP_VERB verb;
   HTTPPath *http_path;
   string body;
   HTTPRequest(const char *str);
   
};