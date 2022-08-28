#pragma once


enum class HTTP_VERB { 

	HTTP_GET = 1, 

	HTTP_POST, 

	HTTP_PUT, 

	HTTP_OPTIONS, 

    HTTP_DELETE,

	HTTP_INVALID_VERB 

};



enum class WellKnowContentType
{
    Json,
    Xml,
    PlainText,
    Html,
    Raw
#if 0
    { "txt", "text/plain" },
    { "css", "text/css" },
    { "csv", "text/csv" },
    { "htm", "text/html" },
    { "html", "text/html" },
    { "xml", "text/xml" },
    { "js", "text/javascript" }, // Technically it should be application/javascript (RFC 4329), but IE8 struggles with that
    { "xhtml", "application/xhtml+xml" },
    { "json", "application/json" },
    { "pdf", "application/pdf" },
    { "zip", "application/zip" },
    { "tar", "application/x-tar" },
    { "gif", "image/gif" },
    { "jpeg", "image/jpeg" },
    { "jpg", "image/jpeg" },
    { "tiff", "image/tiff" },
    { "tif", "image/tiff" },
    { "png", "image/png" },
    { "svg", "image/svg+xml" },
    { "ico", "image/x-icon" },
    { "swf", "application/x-shockwave-flash" },
    { "mp3", "audio/mpeg" },
    { "wav", "audio/x-wav" },
    { "ttf", "font/ttf" },
#endif
};



enum class HTTP_RESPONSE_STATUS {
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

enum  JSON_DATATYPE {
    
    JSON_NUMBER = 1, 
    
    JSON_STRING, 
    
    JSON_OBJECT, 
    
    JSON_ARRAY, 
    
    JSON_BOOLEAN, 
    
    JSON_NULL 
};



/*

audio/mpeg3
audio/x-mpeg-3
video/mpeg
video/x-mpeg
*/