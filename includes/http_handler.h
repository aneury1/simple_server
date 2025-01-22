#ifndef HTTP_HANDLER
#define HTTP_HANDLER
#include "defines.h"
#include <string>
using std::string;
#define INVALID_HTTP_URI "INVALID URL"

enum class request_verb{
    Get,Post,Put,Delete,Options
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
  text_css,
  application_javascript,
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

struct http_request{
   request_verb request_verb_id;
   std::string str_verb;
   std::string url;
   std::unordered_map<std::string, std::string> headers;
   std::unordered_map<std::string, std::string> query_params;
   std::string body;
   int fd;
};

struct http_response{

   string build_http_response(string_map headers,string body, http_response_code statusCode);
   string build_http_response();
   http_response()= default;
   http_response set_body(const std::string& type);
   http_response set_content_type(const http_content_type& type);
   http_response render_text_file(std::string filename);
   http_response render_html_file(std::string filename);
   http_response set_home_path(std::string p){
      home_path = p;
      return (*this);
   } 
   bool close_fd = true;
   const std::string get_home_path()const{
       return home_path;
   }
protected:
   std::string home_path ="/var";
   std::unordered_map<std::string, std::string> headers;
   std::string body;
   http_response_code status_code;
   std::string raw_response;
};
typedef std::function<http_response(http_request)> endpointcb;


class http_server{
   
   mutable std::unordered_map<std::string, endpointcb> routes_map_org;
   std::pair<string, endpointcb> query_endpoint_from_map(const string uri);
   http_server() = default;
   static http_server* server;
   public:

   static http_server* get();

   void handle_connection_request(int con);
   http_server* add_route_handler(std::string url, endpointcb cb);

};



#endif