#include "defines.h"
#include "string_utils.h"
#include "http_handler.h"
#include "io_utils.h"
#include "websocket_handler.h"
#include <sstream>
#include <string>
#include <regex>
using std::string;

std::string default_400_msg()
{
  std::stringstream stream;
  stream << "HTTP/1.1 400 Bad Request\r\n"

         << "Server: SimpleServer/1.0\r\n"
         << "Content-Type: text/html; charset=UTF-8\r\n"
         << "Content-Length: 129\r\n"
         << "Connection: close\r\n\r\n"
         << "<html>"
         << "<head><title>400 Bad Request</title></head>"
         << "<body>"
         << "<h1>400 Bad Request</h1>"
         << "<p>Your request could not be understood by the server due to malformed syntax.</p>"
         << "</body>"
         << "</html>";
  return stream.str();
}
std::string default_500_msg()
{
  std::stringstream stream;
  stream << "HTTP/1.1 500 Not Found\r\n"
         << "Server: SimpleServer/1.0\r\n"
         << "Connection: close\r\n\r\n";
  return stream.str();
}

string extract_http_verb(const string request)
{
  string ret = request.substr(0, request.find_first_of(" "));
  return ret;
}

string http_response_code_to_string(http_response_code code)
{

  auto strBuilder = [](int n, string s)
  {
    return std::to_string(n) + " " + s + "\r\n";
  };

  switch (code)
  {
    // Informational codes (1xx)
  case http_response_code::Continue: // = 100,
    return strBuilder((int)http_response_code::Continue, "Continue");
  case http_response_code::SwitchingProtocols: // = 101,
    return strBuilder((int)http_response_code::SwitchingProtocols,
                      "Switching Protocols");
  // Success codes (2xx)
  case http_response_code::OK: // = 200,
    return strBuilder((int)http_response_code::OK, "OK");
  case http_response_code::Created: // = 201,
    return strBuilder((int)http_response_code::Created, "Created");
  case http_response_code::Accepted: // = 202,
    return strBuilder((int)http_response_code::Accepted, "Accepted");

  // Redirection codes (3xx)
  case http_response_code::MovedPermanently: //  = 301,
    return strBuilder((int)http_response_code::MovedPermanently,
                      "Moved Permanently");
  case http_response_code::Found: // = 302,
    return strBuilder((int)http_response_code::Found, "Found");
  case http_response_code::SeeOther: // = 303,
    return strBuilder((int)http_response_code::SeeOther, "See Other");

  // Client error codes (4xx)
  case http_response_code::BadRequest: //  = 400,
    return strBuilder((int)http_response_code::BadRequest, "Bad Request");
  case http_response_code::Unauthorized: //  = 401,
    return strBuilder((int)http_response_code::Unauthorized, "Unauthorized");
  case http_response_code::Forbidden: //  = 403,
    return strBuilder((int)http_response_code::Forbidden, "OK");
  case http_response_code::NotFound: //  = 404,
    return strBuilder((int)http_response_code::NotFound, "Not Found");

  // Server error codes (5xx)
  case http_response_code::InternalServerError: //  = 500,
    return strBuilder((int)http_response_code::InternalServerError,
                      "Internal Server Error");
  case http_response_code::NotImplemented: //  = 501,
    return strBuilder((int)http_response_code::NotImplemented,
                      "Not Implemented");
  }

  return strBuilder((int)http_response_code::OK, "OK");
}

string content_type_to_string(http_content_type type)
{
  static const std::unordered_map<http_content_type, string> contentTypeMap = {
      {http_content_type::text_html, "text/html"},
      {http_content_type::text_plain, "text/plain"},
      {http_content_type::application_json, "application/json"},
      {http_content_type::application_xml, "application/xml"},
      {http_content_type::multipart_form_data, "multipart/form-data"},
      {http_content_type::application_octet_stream, "application/octet-stream"},
      {http_content_type::image_png, "image/png"},
      {http_content_type::image_jpeg, "image/jpeg"},
      {http_content_type::text_css,"text/css"},
      {http_content_type::application_javascript,"application/javascript"},
      {http_content_type::uknown, "unknown"}};

  auto it = contentTypeMap.find(type);
  if (it != contentTypeMap.end())
  {
    return it->second;
  }
  return "unknown";
}

string http_headers_to_response_string(string_map headers)
{
  string response;
  for (auto it : headers)
    response += it.first + ": " + it.second + "\r\n";
  return response;
}

string extract_url_with_query_params(string request)
{
  int firstSpace = request.find_first_of(" ");
  int findHttpVersion = request.find(" HTTP/1.1");
  string url =
      request.substr(firstSpace + 1, findHttpVersion - (firstSpace + 1));
  return url;
}

string extract_url(string request)
{
  int firstSpace = request.find_first_of(" ");
  int findHttpVersion = request.find(" HTTP/1.1");
  string url =
      request.substr(firstSpace + 1, findHttpVersion - (firstSpace + 1));
  if (url.find_first_of('?') != string::npos)
  {
    return url.substr(0, url.find_first_of("?"));
  }
  return url;
}

string extract_body(const string str)
{
  int pos = str.find("\r\n\r\n");
  if (pos < str.size())
    ;
  return " ";
  string body = str.substr(pos + 2, str.size() - (pos + 2));
  return body;
}

string_map parse_headers(const string &request)
{
  string_map headers;
  std::istringstream iss(request);
  string line;

  std::getline(iss, line);

  while (std::getline(iss, line) && !line.empty())
  {
    size_t colonPos = line.find(':');
    if (colonPos != string::npos)
    {
      string headerName = line.substr(0, colonPos);
      string headerValue = line.substr(colonPos + 2);
      // Remove any leading or trailing whitespaces from the header value
      headerValue.erase(0, headerValue.find_first_not_of(" \t"));
      headerValue.erase(headerValue.find_last_not_of(" \t") + 1);
      headerValue.erase(0, headerValue.find_first_not_of("\r"));
      headerValue.erase(0, headerValue.find_first_not_of("\n"));
      headerValue.erase(headerValue.find_last_not_of("\r\n") + 1);
      headers[headerName] = headerValue;
    }
  }

  return headers;
}

string_map parse_url_params(string url)
{
  string_map ret;
  size_t pos = url.find("?");
  if (pos != string::npos)
  {
    string paramList = url.substr(pos + 1, url.size() - pos);
    auto vars = split_words(paramList, ';');
    for (auto var : vars)
    {
      auto keyPairs = split_words(var, '=');
      ret[keyPairs[0]] = keyPairs[1];
    }
  }
  return ret;
}
url_data_type get_node_data_type_by_pattern(const string &routeNode)
{
  int s = routeNode.size();

  int pos = routeNode.find(":int>");
  if (pos != string::npos)
    return url_data_type::Integer;
  pos = routeNode.find(":str>");
  if (pos != string::npos)
    return url_data_type::Str;
  pos = routeNode.find(":boolean>");
  if (pos != string::npos)
    return url_data_type::Boolean;

  return url_data_type::InvalidUrlDatatype;
}
const string url_data_typeAsStr(const url_data_type &type)
{
  switch (type)
  {
  case url_data_type::Integer:
    return "Integer";
  case url_data_type::Str:
    return "Str";
  case url_data_type::Boolean:
    return "Boolean";
  case url_data_type::InvalidUrlDatatype:
  default:
    return "Invalid";
  }
}

bool is_boolean(string sn)
{
  if (sn.find("true") != string::npos ||
      sn.find("false") != string::npos)
    return true;
  return false;
}

bool is_integer(string sn)
{
  for (auto it : sn)
    if (!isdigit(it))
      return false;
  return true;
}

url_data_type getDataTypeByValue(string value)
{
  if (is_integer(value))
    return url_data_type::Integer;
  if (is_boolean(value))
    return url_data_type::Boolean;
  return url_data_type::Str;
}
bool validate_pattern_by_url(std::vector<string> uri,
                             std::vector<string> pattern)
{

  bool found = false;
  auto uIter = uri.begin();
  auto pIter = pattern.begin();
  auto uIterEnd = uri.end();
  while (uIter != uIterEnd)
  {
    if (*uIter != *pIter)
    {
      string value = *uIter;
      string pValue = *pIter;
      if (pValue[0] != '<')
      {
        return false;
      }
      auto ptype = get_node_data_type_by_pattern(pValue);
      auto vType = getDataTypeByValue(value);
      if (ptype != vType)
        return false;
    }
    else
    {
    }
    pIter++;
    uIter++;
  }
  return true;
};

string build_http_response(string_map headers,
                           string body, http_response_code statusCode)
{
  std::stringstream response;
  response << "HTTP/1.1 " << http_response_code_to_string(statusCode);
  response << http_headers_to_response_string(headers);
  int bodySize = body.size();
  if (bodySize > 0)
  {
    if (headers.find("Content-Length") == headers.end())
      response << "Content-Length: " << bodySize << "\r\n\r\n"
               << body;
    else
      response << "\r\n"
               << body;
  }
  return response.str();
}

request_verb parse_http_request_verb(string buffer)
{
  auto testStr = to_lower_case(buffer);
  if (testStr.find("get") != string::npos)
    return request_verb::Get;
  else if (testStr.find("post") != string::npos)
    return request_verb::Post;
  else if (testStr.find("put") != string::npos)
    return request_verb::Put;
  return request_verb::Get;
}

string http_response::build_http_response(string_map headers, string body, http_response_code statusCode)
{
  std::stringstream response;
  response << "HTTP/1.1 " << http_response_code_to_string(statusCode);
  response << http_headers_to_response_string(headers);
  int bodySize = body.size();
  if (bodySize > 0)
  {
    if (headers.find("Content-Length") == headers.end())
      response << "Content-Length: " << bodySize << "\r\n\r\n"
               << body;
    else
      response << "\r\n"
               << body;
  }
  return response.str();
}

string http_response::build_http_response()
{
  return build_http_response(headers, body, status_code);
}

http_response http_response::set_content_type(const http_content_type &type)
{
  auto str = content_type_to_string(type);
  headers["Content-Type"] = str;
  return (*this);
}

http_response http_response::render_text_file(std::string filename)
{
  auto fi = read_text_file(filename);
  if (fi.empty())
  {
    auto directories = list_dir(filename);
    if (directories.size() > 0)
    {
      for (auto it : directories)
      {
        fi += it + "\r\n";
      }
    }
    else
    {
      fi = "file could not be opened.";
    }
  }
  body = fi;
  return (*this);
}

http_response http_response::render_html_file(std::string filename)
{
  set_content_type(http_content_type::text_html);
  return render_text_file(filename);
}

http_response http_response::set_body(const std::string &body)
{
  headers["Content-Length"] = std::to_string(body.size());
  this->body = body;
  return (*this);
}

http_server *http_server::get()
{
  if (server == nullptr)
  {
    server = new http_server();
    SM_WARN("Creating a new instance of http_server %x", server);
  }
  return server;
}

void http_server::handle_connection_request(int connection)
{
  bool closefd = false;
  string str_request = received_str(connection);
  SM_ERROR("retrieve content length[%d]", str_request.empty());
  if (!str_request.empty())
  {
    auto http_verb = extract_http_verb(str_request);
    auto request_verb = parse_http_request_verb(str_request);
    string urlDynamicPath = extract_url_with_query_params(str_request);
    string url = extract_url(str_request);
    auto headers = parse_headers(str_request);
    string_map queryParams = parse_url_params(urlDynamicPath);
    string body = extract_body(str_request);

    SM_INFO("http verb %s", http_verb.c_str());
    SM_INFO("http url endpoint %s", url.c_str());

    if(websocket_manager::get()->is_websocket(headers)){
       if(websocket_manager::get()->add_new_connection_connection(connection, headers,url)){
         SM_INFO("NEW WEBSOCKET CLIENT");
         return ;
       }
    }
    http_request request;
    request.request_verb_id = request_verb;
    request.str_verb = http_verb;
    request.headers = headers;
    request.url = url;
    request.query_params = queryParams;
    request.body = body;
    bool is_websocket = false;

    string response;
    auto it = query_endpoint_from_map(url);
    SM_INFO("find of endpoint %s", it.first.c_str());
    if (it.first == INVALID_HTTP_URI)
    {
     #if 0
      std::stringstream body;

      body << "HTTP VERB: " << http_verb << "\n"
           << "URL: " << it.first << "\n"
           << "Dynamic Url: " << urlDynamicPath << "\n"
           << "HTTP Headers: ";
      for (auto header : headers)
      {
        body << header.first << ": " << header.second << "\n";
      }
      if (!queryParams.empty())
      {
        body << "Query Parameters: \n";
        for (auto param : queryParams)
        {
          body << param.first << " : " << param.second << "\n";
        }
      }
      response = "HTTP/1.1 200 OK\r\n";
      response +=
          ("Content-Length: ") + std::to_string(body.str().size()) + "\r\n";
      response += "Content-Type: text/plain\r\n";
      response += "\r\n\r\n";
      response += body.str();
      #endif
      response = default_500_msg();
      closefd = true;
    }
    else
    {
      http_response res = it.second(request);
      closefd = res.close_fd;
      response = res.build_http_response();
    }
    int querysend =
        send(connection, response.c_str(), response.length(), 0);
  }

  if (closefd)
  {
    SM_WARN("Closing the socket");
    closesocket(connection);
  }
  else
  {
    SM_WARN("not Closing the socket");
  }
}

http_server *http_server::add_route_handler(std::string url, endpointcb cb)
{
  this->routes_map_org[url] = cb;
  routes_map_org.insert(std::make_pair(url, cb));
  SM_INFO("Adding a route [%s] with memory address [%x] route map length: %ld", url.c_str(), cb, routes_map_org.size());
  return this;
}

http_server *http_server::server = nullptr;

std::pair<string, endpointcb> http_server::query_endpoint_from_map(const string uri)
{
  std::pair<string, endpointcb> res;
  res.first = std::string(INVALID_HTTP_URI);
  if (uri.size() == 1 && uri == "/")
  {
    for (auto it : routes_map_org)
    {
      if (it.first == "/")
        return it;
    }
    return res;
  }

  auto tryfnd = routes_map_org.find(uri);
  if (tryfnd != nullptr)
  {
    SM_TRACE("URI %s found", uri.c_str());
    res.second = tryfnd->second;
    res.first = tryfnd->first;
    return res;
  }
  else
  {
    for (auto it : routes_map_org)
    {
      SM_TRACE("try regex %s on URI", it.first.c_str(), uri.c_str());
      /// std::cout <<"Try regex: "<<it.first<<" On uri: "<<uri<<"\n";
      std::regex pattern(it.first);
      if (std::regex_match(uri, pattern))
      {
        res.first = it.first;
        res.second = it.second;
        return res;
      }
    }
  }

  auto splittedUri = split_words(uri, '/');
  for (auto it : routes_map_org)
  {
    auto eSplitted = split_words(it.first, '/');
    if (eSplitted.size() != splittedUri.size())
      continue;
    bool status = validate_pattern_by_url(splittedUri, eSplitted);
    if (status == true)
      return it;
  }
  SM_TRACE("URI %s  not found, returning default, routes map length: %d", uri.c_str(), routes_map_org.size());
  return res;
}