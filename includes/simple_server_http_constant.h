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

}  // namespace simple_server


#endif