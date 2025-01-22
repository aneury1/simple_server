#ifndef SIMPLE_SERVER_HTTP_CONSTANT_DEFINED
#define SIMPLE_SERVER_HTTP_CONSTANT_DEFINED
#pragma once

namespace simple_server {

enum class content_type {
    text_html,
    text_css,
    text_plain,
    application_json,
    application_xml,
    application_octet_stream,
    image_jpeg,
    image_png,
    audio_mpeg,
    video_mp4,
    unknown
};


enum class http_response_code {
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

enum class http_content_type {
  text_html,
  text_plain,
  application_json,
  application_xml,
  multipart_form_data,
  application_octet_stream,
  image_png,
  image_jpeg,
  uknown,
};

enum class url_data_type{
    Integer,
    Str,
    Boolean,
    InvalidUrlDatatype 
};

enum class request_verb{
    Get,Post,Put,Delete,Options
};

const std::unordered_map<content_type, std::string> content_type_map = {
    {content_type::text_html, "text/html"},
    {content_type::text_plain, "text/plain"},
    {content_type::application_json, "application/json"},
    {content_type::application_xml, "application/xml"},
    {content_type::application_octet_stream, "application/octet-stream"},
    {content_type::image_jpeg, "image/jpeg"},
    {content_type::image_png, "image/png"},
    {content_type::audio_mpeg, "audio/mpeg"},
    {content_type::text_css, "text/css"},
    {content_type::video_mp4, "video/mp4"},
    {content_type::unknown, "unknown"}
};

enum class http_header {
    // HTTP Headers
    Host,
    Content_Type,
    Content_Length,
    User_Agent,
    Accept,
    Authorization,
    Cache_Control,
    Connection,
    
    // WebSocket Headers
    Upgrade,
    Sec_WebSocket_Key,
    Sec_WebSocket_Accept,
    Sec_WebSocket_Version,
    Sec_WebSocket_Protocol,
    Sec_WebSocket_Extensions,
    
    // Default Unknown
    Unknown
};

// Map to convert enum values to lowercase strings
const std::map<http_header, std::string> HeaderToString = {
    // HTTP Headers
    {http_header::Host, "host"},
    {http_header::Content_Type, "content-type"},
    {http_header::Content_Length, "content-length"},
    {http_header::User_Agent, "user-agent"},
    {http_header::Accept, "accept"},
    {http_header::Authorization, "authorization"},
    {http_header::Cache_Control, "cache-control"},
    {http_header::Connection, "connection"},

    // WebSocket Headers
    {http_header::Upgrade, "upgrade"},
    {http_header::Sec_WebSocket_Key, "sec-websocket-key"},
    {http_header::Sec_WebSocket_Accept, "sec-websocket-accept"},
    {http_header::Sec_WebSocket_Version, "sec-websocket-version"},
    {http_header::Sec_WebSocket_Protocol, "sec-websocket-protocol"},
    {http_header::Sec_WebSocket_Extensions, "sec-websocket-extensions"},

    // Default
    {http_header::Unknown, "unknown"}
};


}  // namespace simple_server


#endif