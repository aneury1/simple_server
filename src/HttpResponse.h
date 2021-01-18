#pragma once

///\common response code status.
enum HTTP_RESPONSE_STATUS
 {
     OK_200, 
     CREATE_201, 
     ACCEPTED_202, 
     BAD_RESPONSE_400, 
     FORBIDDEN_403, 
     NOT_FOUND_404,
     METHOD_NOT_ALLOWED_405,
     REQUEST_TIMEOUT_408,
     CONTENT_LENGTH_411,
     TOO_MANY_REQUEST_429,
     INTERNAL_SERVER_ERROR_500,
     NOT_IMPLEMENTED_501,
     BAD_GATEWAY_502
};


///\todo: implement HTTP Response structure.
struct HttpResponse
{

};