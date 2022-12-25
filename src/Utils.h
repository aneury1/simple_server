#pragma once
#include <string>
#include <time.h>
#include <iostream>
#include <fstream>
#include <winsock.h>
#include "HttpRequest.h"
#include "JSON.h"
#include "HttpIdentifier.h"
#include "HttpServer.h"
#include "Typedefs.h"
#include "Constant.h"
#include "HttpResponse.h"
using std::string;


#pragma warning(disable : 4996)

#include <sstream>
#include <string>

static inline std::string convertUrlToFormatter(const std::string& url)
{
    std::stringstream res;
    std::string tmp;
    int ipos = 0;
    int cpos = 0;

    auto cv = [&] {
        if (tmp.length() > 0) {
            if (atol(tmp.c_str()) != 0) {
                res << "/<int>";
            }
            else {
                if (tmp.length() == 1 && tmp[0] == '0')
                {
                    res << "/<int>";
                    return;
                }
                res << "/<str>";
            }
        }
        tmp = "";
    };


    for (auto i : url) {
        if (i == '/') {
            cv();
        }
        else {
            tmp += i;
        }
    }

    cv();
    return res.str();
}


static inline bool compare_ignore_case(string s1, string s2)
{
    if (s1.size() != s2.size())return false;
    if (s1 == s2)return true;
    for (int i = 0;i < s1.length();i++)
    {
        char c1 = tolower(s1[i]), c2 = tolower(s2[i]);
        if (c1 != c2)return false;
    }
    return true;
}

static inline std::string readFile(const char* name) {
    std::fstream stream(name, std::ios::in);
    std::string content((std::istreambuf_iterator<char>(stream)),
        std::istreambuf_iterator<char>()
    );
    return content;
}
#include <vector>
static inline std::vector<unsigned char> readBinaryFile(const char* name) {
    
    std::vector<unsigned char> content;
    // open the file:
    std::ifstream file(name, std::ios::binary);

    // Stop eating new lines in binary mode!!!
    file.unsetf(std::ios::skipws);

    // get its size:
    std::streampos fileSize;

    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // reserve capacity
    std::vector<BYTE> vec;
    vec.reserve(fileSize);
    printf("File size %ld", fileSize);
    // read the data:
    vec.insert(vec.begin(),
        std::istream_iterator<BYTE>(file),
        std::istream_iterator<BYTE>());
    printf("vec size %ld", vec.size());
    return vec;

    
}




static inline char* genGUID()
{
    char buffer[36] = { 0 };
    srand(time(NULL));
    sprintf(buffer, "%x%x-%x-%x-%x-%x%x%x",
        rand(), rand(),		// Generates a 64-bit Hex number
        rand(),				// Generates a 32-bit Hex number
        ((rand() & 0x0fff) | 0x4000),	// Generates a 32-bit Hex number of the form 4xxx (4 indicates the UUID version)
        rand() % 0x3fff + 0x8000,		// Generates a 32-bit Hex number in the range [0x8000, 0xbfff]
        rand(), rand(), rand());		// Generates a 96-bit Hex number
    return buffer;
}



static inline string remove_leading_space(string b) {
    string ret;
    for (int i = 0;i < b.length(); i++)
    {
        if (!isspace(b[i]))
            ret += b[i];
        else
        {
            ret += "<...>";
        }
    }
    return ret;
}


static const inline string HttpMethodToString(HTTP_VERB verb) {
    switch (verb) {
    case HTTP_VERB::HTTP_GET: return "GET";
    case HTTP_VERB::HTTP_POST: return "POST";
    case HTTP_VERB::HTTP_PUT: return "PUT";
    case HTTP_VERB::HTTP_DELETE: return "DELETE";
    case HTTP_VERB::HTTP_OPTIONS: return "OPTIONS";
    case HTTP_VERB::HTTP_INVALID_VERB: return "NOT IMPLEMENTED";
    }
}


static inline string getAllUser()
{
    JSONObject object;
    JsonPair pair1("user_id", std::string("TOK1200034"));
    JsonPair pair2("user_password", "123456");
    JsonPair pair3("user_date_of_registry", "0000-00-00T12:00:00");
    JsonPair pair4("logged", true);
    JsonPair pair5("number", 12);
    JsonPair pair6("float_expression", 12.4f);
    JsonPair pair7("file", readFile("D:\\SDKS\\flutter_windows_3.0.5-stable\\flutter\\README.md"));
    object.addElement(&pair1);
    object.addElement(&pair2);
    object.addElement(&pair3);
    object.addElement(&pair4);
    object.addElement(&pair5);
    object.addElement(&pair6);
    object.addElement(&pair7);
    return object.stringify();
}



static inline void handleHTTPResponse(int client, const char* query, int len)
{
    std::cout << "Without Connection handler\n" << query << "\n\n\n";
    HttpRequest query_request(query,query, client);
    string response_buffer;
    std::string format = convertUrlToFormatter(query_request.path);
    std::cout <<"url->" << format;
    response_buffer += "HTTP/1.1 200 OK\r\n";
    response_buffer += "Content-Type: application/json\r\n";
    string body_response;
    body_response = getAllUser();
    response_buffer += "Content-Length:" + std::to_string(body_response.length());
    response_buffer += "\r\n\r\n";
    response_buffer += body_response.c_str();
    int rc = send(client, response_buffer.c_str(), response_buffer.size(), 0);
}

static inline void handleHTTPResponseWithMiddleWare(int client, const char* query, int len,MiddleWareContainer middleware)
{
    HttpRequest query_request(query,query,client);
    HttpIdentifier identifier(query_request.path, query_request.verb);
    http_request_handler handler = middleware[identifier];
    if (handler)
    {
        handler(query_request);
    }
    else
    {
        std::cout << "this handler is null redireting to default handler";
        handleHTTPResponse(client, query, len);
    }
}



static const inline std::string getContentTypeString(WellKnowContentType type)
{
    switch (type)
    {
    case WellKnowContentType::Json:
        return "application/json";
    case WellKnowContentType::Xml:
        return "application/xml";
    case WellKnowContentType::PlainText:
        return "text/plain";
    case WellKnowContentType::Html:
        return "text/html";
    case WellKnowContentType::Raw:
    default:
        return "text/plain";
    }
}



static const inline HttpResponse createResponse(HTTP_RESPONSE_STATUS status, std::string payload, WellKnowContentType contentType)
{
    HttpResponse response;
    response.status = status;
    response.payload = payload;
    response.contentType = contentType;
    return response;
}
 
static const std::string getResponseStringFromStatusCode(HTTP_RESPONSE_STATUS status)
{
    switch (status)
    {
    case HTTP_RESPONSE_STATUS::OK_200:
        return "OK";
    case HTTP_RESPONSE_STATUS::CREATE_201:
        return "CREATED";
    case HTTP_RESPONSE_STATUS::ACCEPTED_202:
        return "ACCEPTED";
    case HTTP_RESPONSE_STATUS::BAD_RESPONSE_400:
        return "BAD RESPONSE";
    case HTTP_RESPONSE_STATUS::FORBIDDEN_403:
        return "FORBIDDEN";
    case HTTP_RESPONSE_STATUS::NOT_FOUND_404:
        return "NOT FOUND";
    case HTTP_RESPONSE_STATUS::METHOD_NOT_ALLOWED_405:
        return "METOHOD NOT ALLOWED";
    case HTTP_RESPONSE_STATUS::REQUEST_TIMEOUT_408:
        return "REQUEST TIMEOUT";
    case HTTP_RESPONSE_STATUS::CONTENT_LENGTH_411:
        return "CONTENT LENGTH INVALID";
    case HTTP_RESPONSE_STATUS::TOO_MANY_REQUEST_429:
        return "TOO MANY REQUEST";
    case HTTP_RESPONSE_STATUS::INTERNAL_SERVER_ERROR_500:
        return "INTERNAL SERVER ERROR";
    case HTTP_RESPONSE_STATUS::NOT_IMPLEMENTED_501:
        return "NOT IMPLEMENTED";
    case HTTP_RESPONSE_STATUS::BAD_GATEWAY_502:
    default:
        return "BAD GATEWAY";
    }
}



static const int getResponseStatusCode(HTTP_RESPONSE_STATUS status)
{
    return static_cast<int>(status);
#if 0
    switch (status)
    {
    case HTTP_RESPONSE_STATUS::OK_200:
        return 200;
    case HTTP_RESPONSE_STATUS::CREATE_201:
        return 201;
    case HTTP_RESPONSE_STATUS::ACCEPTED_202:
        return 201;
    case HTTP_RESPONSE_STATUS::BAD_RESPONSE_400:
        return "BAD RESPONSE";
    case HTTP_RESPONSE_STATUS::FORBIDDEN_403:
        return "FORBIDDEN";
    case HTTP_RESPONSE_STATUS::NOT_FOUND_404:
        return "NOT FOUND";
    case HTTP_RESPONSE_STATUS::METHOD_NOT_ALLOWED_405:
        return "METOHOD NOT ALLOWED";
    case HTTP_RESPONSE_STATUS::REQUEST_TIMEOUT_408:
        return "REQUEST TIMEOUT";
    case HTTP_RESPONSE_STATUS::CONTENT_LENGTH_411:
        return "CONTENT LENGTH INVALID";
    case HTTP_RESPONSE_STATUS::TOO_MANY_REQUEST_429:
        return "TOO MANY REQUEST";
    case HTTP_RESPONSE_STATUS::INTERNAL_SERVER_ERROR_500:
        return "INTERNAL SERVER ERROR";
    case HTTP_RESPONSE_STATUS::NOT_IMPLEMENTED_501:
        return "NOT IMPLEMENTED";
    case HTTP_RESPONSE_STATUS::BAD_GATEWAY_502:
        return "BAD GATEWAY";
    default:
        return "ERROR NOT HANDLED BY THE SERVER";
    }
#endif
}


static inline
std::string createHttpResponse(int statusCode, std::string message, WellKnowContentType contentType, std::string payload)
{
    string response_buffer;
    response_buffer += "HTTP/1.1";
    response_buffer += std::to_string(statusCode);
    response_buffer += message;
    response_buffer += "\r\n";
    response_buffer += "Content-Type: ";
    response_buffer += getContentTypeString(contentType);
    response_buffer += "\r\n";
    response_buffer += "Content-Length:" + std::to_string(payload.length());
    response_buffer += "\r\n\r\n";
    response_buffer += payload.c_str();
    return response_buffer;
}


static inline
std::string createHttpResponseFromPayload(const HttpResponse& response)
{
    string response_buffer;
    response_buffer += "HTTP/1.1";
    response_buffer += std::to_string(getResponseStatusCode(response.status));
    response_buffer += getResponseStringFromStatusCode(response.status);
    response_buffer += "\r\n";
    response_buffer += "Content-Type: ";
    response_buffer += getContentTypeString(response.contentType);
    response_buffer += "\r\n";
    response_buffer += "Content-Length:" + std::to_string(response.payload.length());
    response_buffer += "\r\n\r\n";
    response_buffer += response.payload.c_str();
    return response_buffer;
}
 


/// <summary>
///  This function is taken from: 
///  https://github.com/mattgodbolt/seasocks/blob/master/src/main/c/internal/Base64.cpp
/// // Copyright (c) 2013-2017, Matt Godbolt
//   All rights reserved.
/// </summary>
namespace {
    const char cb64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
}

static const inline std::string base64Encode(const void* data, size_t length) {
    std::string output;
    const auto dataPtr = reinterpret_cast<const uint8_t*>(data);
    for (auto i = 0u; i < length; i += 3) {
        const auto bytesLeft = length - i;
        const auto b0 = dataPtr[i];
        const auto b1 = bytesLeft > 1 ? dataPtr[i + 1] : 0;
        const auto b2 = bytesLeft > 2 ? dataPtr[i + 2] : 0;
        output.push_back(cb64[b0 >> 2]);
        output.push_back(cb64[((b0 & 0x03) << 4) | ((b1 & 0xf0) >> 4)]);
        output.push_back((bytesLeft > 1 ? cb64[((b1 & 0x0f) << 2) | ((b2 & 0xc0) >> 6)] : '='));
        output.push_back((bytesLeft > 2 ? cb64[b2 & 0x3f] : '='));
    }
    return output;
}

static inline std::string toLowerCaseString(std::string s) {
    std::stringstream ret;
    for (auto it : s)
        ret << std::tolower(it);
    return ret.str();
}

static inline UriScheme getScheme(std::string url) {
    url = toLowerCaseString(url);
    std::string::iterator it = url.begin();
    if (std::equal(it, it + 6, "ws://"))return UriScheme::Wss;
    if (std::equal(it, it + 8, "https://"))return UriScheme::Https;
    if (std::equal(it, it + 7, "http://"))return UriScheme::Http;
    return UriScheme::Uknown;
}

static inline void GenGUIDId(char *uidCtx) {

    srand(time(NULL));
    sprintf(uidCtx, "%x%x-%x-%x-%x-%x%x%x",
        rand(), rand(),		// Generates a 64-bit Hex number
        rand(),				// Generates a 32-bit Hex number
        ((rand() & 0x0fff) | 0x4000),	// Generates a 32-bit Hex number of the form 4xxx (4 indicates the UUID version)
        rand() % 0x3fff + 0x8000,		// Generates a 32-bit Hex number in the range [0x8000, 0xbfff]
        rand(), rand(), rand());		// Generates a 96-bit Hex number
     
}


struct Buffer {
    unsigned char* buffer;
    int length;
};

Buffer* bufferFrom(unsigned char* ptr, int len) {
    Buffer* ret = (Buffer*)malloc(len);
    memset(ret, 0x00, len);
    memcpy(ret, ptr, len);
    return ret;
}

Buffer* bufferFrom(Buffer* buffer) {
    if (buffer)
        return bufferFrom(buffer->buffer, buffer->length);
    return NULL;
}

