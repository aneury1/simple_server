#ifndef _HTTPRESPONSE_H_DEFINED
#define _HTTPRESPONSE_H_DEFINED
#include "Constant.h"
#include <string>
struct HttpResponse
{
    HTTP_RESPONSE_STATUS status;
    std::string payload;
    WellKnowContentType contentType;
};

#endif