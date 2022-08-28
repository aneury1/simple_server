#ifndef _HTTPRESPONSE_H_DEFINED
#define _HTTPRESPONSE_H_DEFINED
#include "Constant.h"
#include "SocketServerBase.h"
#include <string>
struct HttpResponse
{
    HTTP_RESPONSE_STATUS status;
    std::string payload;
    WellKnowContentType contentType;
    SocketServer::SocketFd client;

    HttpResponse write(std::string payload);
    HttpResponse writeHeaders(std::string key, std::string value);
    HttpResponse write(unsigned char* payload, int length);

private:
   /* HttpResponse() {

    }
    HttpResponse(const HttpResponse& res) = delete;
    friend class HttpServer;*/
};

#endif