#include "HttpPrimitiveUtils.h"
#include "IOUtils.h"
#include <sstream>

#define TEST_STATIC_FILE "index.html"
#define DEFAULT_FAV_ICON_PATH                                                  \
  "/home/aneury/Downloads/favicon.ico" /// TODO fix as required

Response *getFavIcon(Request *request) {
  Response *resp = new Response();
  resp->body = readWholeFile(DEFAULT_FAV_ICON_PATH);
  resp->headers["image/x-icon"];
  resp->headers["Content-Length"] = std::to_string(resp->body.size());
  return resp;
}

/**
 * this function will convert string to lower case.
 */
std::string toLowerCaseString(const std::string &str) {
  std::string ret = str;
  std::transform(ret.begin(), ret.end(), ret.begin(),
                 [](char s) { return std::tolower(s); });
  return ret;
}

RequestVerb parseRequestVerb(std::string buffer) {
  auto testStr = toLowerCaseString(buffer);
  if (testStr.find("get") != std::string::npos)
    return RequestVerb::Get;
  else if (testStr.find("post") != std::string::npos)
    return RequestVerb::Post;
  return RequestVerb::Get;
}

std::string RequestVerbToString(RequestVerb httpVerb) {
  switch (httpVerb) {
  case RequestVerb::Get:
    return "GET";
  case RequestVerb::Post:
    return "POST";
  default:
    return "UNDEFINED";
  }
}

/// @brief this function help to parse url params like /host?id=1;name=aneury
///        this would convert to map: map[id]=1, map[name]=aneury
/// @return unordered_map<string,string>
std::unordered_map<std::string, std::string> parseUrlParams(std::string url) {
  std::unordered_map<std::string, std::string> ret;
  size_t pos = url.find("?");
  if (pos != std::string::npos) {
    std::string paramList = url.substr(pos + 1, url.size() - pos);
    auto vars = splitWords(paramList, ';');
    for (auto var : vars) {
      auto keyPairs = splitWords(var, '=');
      ret[keyPairs[0]] = keyPairs[1];
    }
  }
  return ret;
}

/// @brief extract Get, Post Delete Request
/// @param request
/// @return
std::string extractHttpVerb(const std::string request) {
  std::string ret = request.substr(0, request.find_first_of(" "));
  return ret;
}

/// @brief Extract current Url
/// @param request
/// @return
std::string extractUrlWithQueryParams(std::string request) {
  int firstSpace = request.find_first_of(" ");
  int findHttpVersion = request.find(" HTTP/1.1");
  std::string url =
      request.substr(firstSpace + 1, findHttpVersion - (firstSpace + 1));
  return url;
}

/// @brief Extract current Url
/// @param request
/// @return
std::string extractUrl(std::string request) {
  int firstSpace = request.find_first_of(" ");
  int findHttpVersion = request.find(" HTTP/1.1");
  std::string url =
      request.substr(firstSpace + 1, findHttpVersion - (firstSpace + 1));
  if (url.find_first_of('?') != std::string::npos) {
    return url.substr(0, url.find_first_of("?"));
  }
  return url;
}

/// @brief  Extract Content Body...
/// @param str
/// @return
std::string extractBody(const std::string str) {
  int pos = str.find("\r\n\r\n");
  if (pos < str.size())
    ;
  return " ";
  std::string body = str.substr(pos + 2, str.size() - (pos + 2));
  return body;
}
/// @brief Parse Http Header..
/// @param request
/// @return
std::unordered_map<std::string, std::string>
parseHeaders(const std::string &request) {
  std::unordered_map<std::string, std::string> headers;

  std::istringstream iss(request);
  std::string line;
  // Skip the first line (request line)
  std::getline(iss, line);

  while (std::getline(iss, line) && !line.empty()) {
    size_t colonPos = line.find(':');
    if (colonPos != std::string::npos) {
      std::string headerName = line.substr(0, colonPos);
      std::string headerValue =
          line.substr(colonPos + 2); // Skip the colon and space

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

ClientInfo::~ClientInfo() {
  int fd = sockfd;
  close(sockfd);
  std::cout << "Client" << fd << " Destroyed\n";
}

ParserEndpoint parseRequest(Request &request, ParserEndpoint notFoundHandler) {
  return nullptr;
}

// Function to handle an individual client connection
void handleClientInfoFromThread(std::shared_ptr<ClientInfo> client) {
  char buffer[8194] = {0x00};
  int bytesRead = recv(client->sockfd, buffer, sizeof(buffer), 0);

  if (bytesRead > 0) {
    std::string request(buffer, bytesRead);

    std::string httpVerb = extractHttpVerb(request);
    auto requestVerb = parseRequestVerb(request);
    std::string urlDynamicPath = extractUrlWithQueryParams(request);
    std::string url = extractUrl(request);
    auto headers = parseHeaders(request);
    std::unordered_map<std::string, std::string> queryParams =
        parseUrlParams(urlDynamicPath);
    std::string body = extractBody(request);
    auto it = getEndpointFromMap(url, registered_endpoint);

    std::string response;

    if (it.first == INVALID_HTTP_URI) {
      std::stringstream body;

      body << "HTTP VERB: " << httpVerb << "\n"
           << "URL: " << url << "\n"
           << "Dynamic Url: " << urlDynamicPath << "\n"
           << "HTTP Headers: ";
      for (auto header : headers) {
        body << header.first << ": " << header.second << "\n";
      }
      if (queryParams.size() > 0) {
        body << "Query Parameters: \n";
        for (auto param : queryParams) {
          body << param.first << " : " << param.second << "\n";
        }
      }
      response = "HTTP/1.1 200 OK\r\n";
      response +=
          ("Content-Length: ") + std::to_string(body.str().size()) + "\r\n";
      response += "Content-Type: text/plain\r\n";
      response += "\r\n\r\n";
      response += body.str();
    } else {
      Request request;
      request.requestVerb = requestVerb;
      request.headers = headers;
      request.url = url;
      request.queryParams = queryParams;
      request.body = body;
      auto res = it.second(&request);
      response = res->buildResponse();
    }
    int querysend =
        send(client->sockfd, response.c_str(), response.length(), 0);
  }

#if 0  
    // Receive and process the request
    char buffer[8194] = {0x00};
    int bytesRead = recv(client->sockfd, buffer, sizeof(buffer), 0);

    if (bytesRead > 0) {
        std::string request(buffer, bytesRead);

        auto headers = parseHeaders(request);
        auto head = parseRequest(request);
        Request thisRequest;
        thisRequest.url = head["url"];
        thisRequest.parameters = headers;
        thisRequest.requestVerb = parseRequestVerb(buffer);
        thisRequest.body = extractBody(buffer);

        auto res = parseRequest(thisRequest, nullptr);

        std::string stream;


        auto workedResponse = res(&thisRequest);

        // Process the request and generate a response
        std::string response = "HTTP/1.1 200 OK\r\n";
        response += ("Content-Length: ") + workedResponse->headers["Content-Length"] + "\r\n";
        response += ("Content-Type: " + workedResponse->headers["Content-Type"]);
        response += "\r\n\r\n";
        response += workedResponse->body;
        //12\r\n\r\nHello World!";

        // Send the response back to the client
        int querysend = send(client->sockfd, response.c_str(), response.length(), 0);
    }

    // Close the client connection
    closesocket(client->sockfd);
    delete client;
#endif
}

#include <map>
#include <sstream>
#include <string>

std::string fetchJsBuilder(std::string url, std::string body,
                           std::map<std::string, std::string> fetchOptions,
                           std::map<std::string, std::string> httpHeaders) {

  std::stringstream fetchOptsStream;

  fetchOptsStream << "async function postData(url = \"\", data = {}) {\r\n";
  fetchOptsStream << "const response = await fetch(\"" << url << "\",{\r\n";
  for (auto it : fetchOptions) {
    fetchOptsStream << it.first << ": \"" << it.second << "\",\r\n\t";
  }

  if (httpHeaders.size() > 0) {
    fetchOptsStream << "headers: {";
    for (auto it : httpHeaders)
      fetchOptsStream << "\"" << it.first << "\":"
                      << "\"" << it.first << "\",\r\n\t";
    fetchOptsStream << "masterc:1\r\n}";
  }
  if (body.size() > 0) {
    fetchOptsStream << "\r\n,body: "
                    << "JSON.stringify("
                    << "{}"
                    << ")});";
  } else {
    fetchOptsStream << ",});\r\n";
  }
  fetchOptsStream << "\r\nreturn response.json(); \r\n}\r\n";
  return fetchOptsStream.str();
#if 0
 const response = await fetch(url, {
    method: "POST", // *GET, POST, PUT, DELETE, etc.
    mode: "cors", // no-cors, *cors, same-origin
    cache: "no-cache", // *default, no-cache, reload, force-cache, only-if-cached
    credentials: "same-origin", // include, *same-origin, omit
    headers: {
      "Content-Type": "application/json",
      // 'Content-Type': 'application/x-www-form-urlencoded',
    },
    redirect: "follow", // manual, *follow, error
    referrerPolicy: "no-referrer", // no-referrer, *no-referrer-when-downgrade, origin, origin-when-cross-origin, same-origin, strict-origin, strict-origin-when-cross-origin, unsafe-url
    body: JSON.stringify(data), // body data type must match "Content-Type" header
  });
  return response.json(); // parses JSON response into native JavaScript objects
#endif
}

Response *DefaultEndpoint(Request *request) {
  Response *ret = new Response();
  ret->body = "NO RESPONSE MSG";
  ret->headers["Content-type"] = "text/plain";
  ret->statusCode = HttpResponseCode::NotFound;
  return ret;
}
Response *DefaultEndpointW200(Request *request) {
  Response *ret = new Response();
  ret->body = "NO RESPONSE MSG";
  ret->headers["Content-type"] = "text/plain";
  ret->statusCode = HttpResponseCode::OK;
  return ret;
}

UrlDatatype getNodeDataTypeByPattern(const std::string &routeNode) {
  int s = routeNode.size();

  int pos = routeNode.find(":int>");
  if (pos != std::string::npos)
    return UrlDatatype::Integer;
  pos = routeNode.find(":str>");
  if (pos != std::string::npos)
    return UrlDatatype::Str;
  pos = routeNode.find(":boolean>");
  if (pos != std::string::npos)
    return UrlDatatype::Boolean;

  return UrlDatatype::InvalidUrlDatatype;
}
const std::string UrlDatatypeAsStr(const UrlDatatype &type) {
  switch (type) {
  case UrlDatatype::Integer:
    return "Integer";
  case UrlDatatype::Str:
    return "Str";
  case UrlDatatype::Boolean:
    return "Boolean";
  case UrlDatatype::InvalidUrlDatatype:
  default:
    return "Invalid";
  }
}

bool isBoolean(std::string sn) {
  if (sn.find("true") != std::string::npos ||
      sn.find("false") != std::string::npos)
    return true;
  return false;
}

bool isInteger(std::string sn) {
  for (auto it : sn)
    if (!isdigit(it))
      return false;
  return true;
}

UrlDatatype getDataTypeByValue(std::string value) {
  if (isInteger(value))
    return UrlDatatype::Integer;
  if (isBoolean(value))
    return UrlDatatype::Boolean;
  return UrlDatatype::Str;
}
std::vector<std::string> splitWords(std::string word, char sp) {
  std::vector<std::string> ret;
  int iter = 0;
  std::string str;
  while (word[iter] != '\0') {

    if (word[iter] != sp) {
      str += word[iter];
    } else {

      if (str.size() > 0) {
        if (word[iter] == sp) {
          ret.push_back(str);
          str = "";
        } else if (word[iter + 1] == sp) {
          str += word[iter];
          ret.push_back(str);
          str = "";
        }
      }
    }
    iter++;
  }
  if (str.size() > 0)
    ret.push_back(str);
  return ret;
};

bool validatePatterByUrl(std::vector<std::string> uri,
                         std::vector<std::string> pattern) {

  bool found = false;
  auto uIter = uri.begin();
  auto pIter = pattern.begin();
  auto uIterEnd = uri.end();
  while (uIter != uIterEnd) {
    if (*uIter != *pIter) {
      std::string value = *uIter;
      std::string pValue = *pIter;
      if (pValue[0] != '<') {
        return false;
      }
      auto ptype = getNodeDataTypeByPattern(pValue);
      auto vType = getDataTypeByValue(value);
      if (ptype != vType)
        return false;
    } else {
    }
    pIter++;
    uIter++;
  }
  return true;
};

std::pair<std::string, ParserEndpoint>
getEndpointFromMap(const std::string uri,
                   const std::map<std::string, ParserEndpoint> &enpointList) {

  std::pair<std::string, ParserEndpoint> res;
  res.first = INVALID_HTTP_URI;

  if (uri.size() == 1 && uri == "/") {
    std::string i = uri.c_str();
    for (auto it : enpointList) {
      if (it.first == "/")
        return it;
    }
    return res;
  }

  auto splittedUri = splitWords(uri, '/');
  for (auto it : enpointList) {
    auto eSplitted = splitWords(it.first, '/');
    if (eSplitted.size() != splittedUri.size())
      continue;
    bool status = validatePatterByUrl(splittedUri, eSplitted);
    if (status == true)
      return it;
  }
  return res;
}

std::string generateStrRequestPaylod(const std::string &path,
                                     const RequestVerb &verb) {
#ifdef VERSION_2_0
  // std::string secWebSocketKey = generateRandomString(16);
  // std::string secWebSocketAccept = generateSHA1Hash(secWebSocketKey +
  // "258EAFA5-E914-47BE-9B52-6C18FB827A3A"); // Magic string
#endif
  const std::string hostname = "localhost";
  const std::string port = "9090";
  std::string request =
      RequestVerbToString(verb) + " " + path + " HTTP/1.1\r\n";
  request += "Upgrade: websocket\r\n";
  request += "Connection: Upgrade\r\n";
  request += "Host: " + hostname + ":" + port + "\r\n";
  request += "supper-long-lasting-information-parameter: "
             "258EAFA5-E914-47BE-9B52-6C18FB827A3A\r\n";
#ifdef VERSION_2_0
  request += "Sec-WebSocket-Key: " + secWebSocketKey + "\r\n";
  request += "Sec-WebSocket-Accept: " + secWebSocketAccept + "\r\n";
#endif
  // Add other optional headers if needed

  request += "\r\n";
  return request;
}

std::string HttpResponseCodeToResponseString(HttpResponseCode code) {

  auto strBuilder = [](int n, std::string s) {
    return std::to_string(n) + " " + s + "\r\n";
  };

  switch (code) {
    // Informational codes (1xx)
  case HttpResponseCode::Continue: // = 100,
    return strBuilder((int)HttpResponseCode::Continue, "Continue");
  case HttpResponseCode::SwitchingProtocols: // = 101,
    return strBuilder((int)HttpResponseCode::SwitchingProtocols,
                      "Switching Protocols");
  // Success codes (2xx)
  case HttpResponseCode::OK: // = 200,
    return strBuilder((int)HttpResponseCode::OK, "OK");
  case HttpResponseCode::Created: // = 201,
    return strBuilder((int)HttpResponseCode::Created, "Created");
  case HttpResponseCode::Accepted: // = 202,
    return strBuilder((int)HttpResponseCode::Accepted, "Accepted");

  // Redirection codes (3xx)
  case HttpResponseCode::MovedPermanently: //  = 301,
    return strBuilder((int)HttpResponseCode::MovedPermanently,
                      "Moved Permanently");
  case HttpResponseCode::Found: // = 302,
    return strBuilder((int)HttpResponseCode::Found, "Found");
  case HttpResponseCode::SeeOther: // = 303,
    return strBuilder((int)HttpResponseCode::SeeOther, "See Other");

  // Client error codes (4xx)
  case HttpResponseCode::BadRequest: //  = 400,
    return strBuilder((int)HttpResponseCode::BadRequest, "Bad Request");
  case HttpResponseCode::Unauthorized: //  = 401,
    return strBuilder((int)HttpResponseCode::Unauthorized, "Unauthorized");
  case HttpResponseCode::Forbidden: //  = 403,
    return strBuilder((int)HttpResponseCode::Forbidden, "OK");
  case HttpResponseCode::NotFound: //  = 404,
    return strBuilder((int)HttpResponseCode::NotFound, "Not Found");

  // Server error codes (5xx)
  case HttpResponseCode::InternalServerError: //  = 500,
    return strBuilder((int)HttpResponseCode::InternalServerError,
                      "Internal Server Error");
  case HttpResponseCode::NotImplemented: //  = 501,
    return strBuilder((int)HttpResponseCode::NotImplemented, "Not Implemented");
  }

  return strBuilder((int)HttpResponseCode::OK, "OK");
}

std::string httpHeadersToResponseString(
    std::unordered_map<std::string, std::string> headers) {
  std::string response;
  for (auto it : headers)
    response += it.first + ": " + it.second + "\r\n";
  return response;
}

std::string
buildHttpResponseResponse(std::unordered_map<std::string, std::string> headers,
                          std::string body, HttpResponseCode statusCode) {
  std::stringstream response;
  response << "HTTP/1.1 " << HttpResponseCodeToResponseString(statusCode);
  response << httpHeadersToResponseString(headers);
  int bodySize = body.size();
  if (bodySize > 0) {
    if (headers.find("Content-Length") == headers.end())
      response << "Content-Length: " << bodySize << "\r\n\r\n" << body;
    else
      response << "\r\n" << body;
  }
  return response.str();
}

std::string Response::buildResponse() {
  return buildHttpResponseResponse(headers, body, statusCode);
}

std::string ContentTypetoString(HttpContentType type) {
  static const std::unordered_map<HttpContentType, std::string> contentTypeMap =
      {{HttpContentType::TEXT_HTML, "text/html"},
       {HttpContentType::TEXT_PLAIN, "text/plain"},
       {HttpContentType::APPLICATION_JSON, "application/json"},
       {HttpContentType::APPLICATION_XML, "application/xml"},
       {HttpContentType::MULTIPART_FORM_DATA, "multipart/form-data"},
       {HttpContentType::APPLICATION_OCTET_STREAM, "application/octet-stream"},
       {HttpContentType::IMAGE_PNG, "image/png"},
       {HttpContentType::IMAGE_JPEG, "image/jpeg"},
       {HttpContentType::UNKNOWN, "unknown"}};

  auto it = contentTypeMap.find(type);
  if (it != contentTypeMap.end()) {
    return it->second;
  }
  return "unknown";
}