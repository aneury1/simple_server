#include "simple_server_http_connection.h"
#include "simple_server_definitions.h"
#include "simple_server_http_constant.h"
#include "simple_server_utils.h"

namespace simple_server {

Response::Response(){

}

String http_headers_to_response_string(
    StringMap headers) {
  String response;
  for (auto it : headers)
    response += it.first + ": " + it.second + "\r\n";
  return response;
}

String build_http_response(StringMap headers,
                          String body, http_response_code statusCode) {
  StringStream response;
  response << "HTTP/1.1 " << http_response_code_to_string(statusCode);
  response << http_headers_to_response_string(headers);
  int bodySize = body.size();
  if (bodySize > 0) {
    if (headers.find("Content-Length") == headers.end())
      response << "Content-Length: " << bodySize << "\r\n\r\n" << body;
    else
      response << "\r\n" << body;
  }
  return response.str();
}


String Response::buildResponse() {
  return build_http_response(headers, body, status_code);
}

String content_type_to_string(http_content_type type) {
  static const std::unordered_map<http_content_type, String> contentTypeMap = {
      {http_content_type::text_html, "text/html"},
      {http_content_type::text_plain, "text/plain"},
      {http_content_type::application_json, "application/json"},
      {http_content_type::application_xml, "application/xml"},
      {http_content_type::multipart_form_data, "multipart/form-data"},
      {http_content_type::application_octet_stream, "application/octet-stream"},
      {http_content_type::image_png, "image/png"},
      {http_content_type::image_jpeg, "image/jpeg"},
      {http_content_type::uknown, "unknown"}};

  auto it = contentTypeMap.find(type);
  if (it != contentTypeMap.end()) {
    return it->second;
  }
  return "unknown";
}

String http_response_code_to_string(http_response_code code) {

  auto strBuilder = [](int n, String s) {
    return std::to_string(n) + " " + s + "\r\n";
  };

  switch (code) {
      // Informational codes (1xx)
    case http_response_code::Continue:  // = 100,
      return strBuilder((int)http_response_code::Continue, "Continue");
    case http_response_code::SwitchingProtocols:  // = 101,
      return strBuilder((int)http_response_code::SwitchingProtocols,
                        "Switching Protocols");
    // Success codes (2xx)
    case http_response_code::OK:  // = 200,
      return strBuilder((int)http_response_code::OK, "OK");
    case http_response_code::Created:  // = 201,
      return strBuilder((int)http_response_code::Created, "Created");
    case http_response_code::Accepted:  // = 202,
      return strBuilder((int)http_response_code::Accepted, "Accepted");

    // Redirection codes (3xx)
    case http_response_code::MovedPermanently:  //  = 301,
      return strBuilder((int)http_response_code::MovedPermanently,
                        "Moved Permanently");
    case http_response_code::Found:  // = 302,
      return strBuilder((int)http_response_code::Found, "Found");
    case http_response_code::SeeOther:  // = 303,
      return strBuilder((int)http_response_code::SeeOther, "See Other");

    // Client error codes (4xx)
    case http_response_code::BadRequest:  //  = 400,
      return strBuilder((int)http_response_code::BadRequest, "Bad Request");
    case http_response_code::Unauthorized:  //  = 401,
      return strBuilder((int)http_response_code::Unauthorized, "Unauthorized");
    case http_response_code::Forbidden:  //  = 403,
      return strBuilder((int)http_response_code::Forbidden, "OK");
    case http_response_code::NotFound:  //  = 404,
      return strBuilder((int)http_response_code::NotFound, "Not Found");

    // Server error codes (5xx)
    case http_response_code::InternalServerError:  //  = 500,
      return strBuilder((int)http_response_code::InternalServerError,
                        "Internal Server Error");
    case http_response_code::NotImplemented:  //  = 501,
      return strBuilder((int)http_response_code::NotImplemented,
                        "Not Implemented");
  }

  return strBuilder((int)http_response_code::OK, "OK");
}

String extract_http_verb(const String request) {
  String ret = request.substr(0, request.find_first_of(" "));
  return ret;
}

request_verb parseRequestVerb(String buffer) {
  auto testStr = to_lower_case_str(buffer);
  if (testStr.find("get") != String::npos)
    return request_verb::Get;
  else if (testStr.find("post") != String::npos)
    return request_verb::Post;
  else if (testStr.find("put") != String::npos)
    return request_verb::Put;
  return request_verb::Get;
}

// Function to convert content_type to string
std::string http_content_type_to_string(content_type type) {
    auto it = content_type_map.find(type);
    if (it != content_type_map.end()) {
        return it->second;
    }
    return "unknown";
}

// Function to get content_type from string
content_type http_content_type_from_string(const std::string& type) {
    for (const auto& pair : content_type_map) {
        if (pair.second == type) {
            return pair.first;
        }
    }
    return content_type::unknown;
}



}  // namespace simple_server