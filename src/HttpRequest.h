#pragma once
#include <string>
using std::string;
#include <vector>
#include "HTTPPath.h"


///\this is the list of HTTP Verb that server support.
enum class HTTP_VERB{HTTP_GET = 1, HTTP_POST, HTTP_PUT, HTTP_OPTIONS,HTTP_INVALID_VERB};

enum class WellKnowContentType
{
    Json,
    Xml,
    PlainText,
    Html,
    Raw
};




///\placeholder structure for entry in the http request.
struct HttpHeader
{
    string key;
    string value;
    HttpHeader(string bulk);
    HttpHeader(string key, string value);
    string toString();
};

//\the structure that handle the http request.
struct HTTPRequest
{
   std::vector<HttpHeader> headers;
   string path;
   HTTP_VERB verb;
   HTTPPath *http_path;
   string body;
   HTTPRequest(const char *str);
};