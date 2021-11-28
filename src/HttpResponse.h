#pragma once

#include <string>

#include "HttpRequest.h"

///\common response code status.
enum class HTTP_RESPONSE_STATUS
 {
     OK_200 = 200, 
     CREATE_201 = 201, 
     ACCEPTED_202 = 202, 
     BAD_RESPONSE_400  = 400,  
     FORBIDDEN_403 = 403, 
     NOT_FOUND_404 = 404,
     METHOD_NOT_ALLOWED_405= 405,
     REQUEST_TIMEOUT_408 = 408,
     CONTENT_LENGTH_411= 411,
     TOO_MANY_REQUEST_429 = 429,
     INTERNAL_SERVER_ERROR_500 = 500,
     NOT_IMPLEMENTED_501= 501,
     BAD_GATEWAY_502 = 502
};


///\todo: implement HTTP Response structure.
struct HttpResponse
{
    HTTP_RESPONSE_STATUS status;
    std::string payload;
    WellKnowContentType contentType;
};

static const HttpResponse createResponse(HTTP_RESPONSE_STATUS status, std::string payload, WellKnowContentType contentType)
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
