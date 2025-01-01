#include <regex>
#include <thread>
#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#define closesocket close
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include "simple_server_definitions.h"
#include "simple_server_http_connection.h"
#include "simple_server_http_constant.h"
#include "simple_server_interface.h"
#include "simple_server_http_server.h"
#include "simple_server_logger.h"
#include "simple_server_utils.h"

namespace simple_server {

 
server_status create_socket_fd(socket_connection& fd) {
  fd.connection = socket(AF_INET, SOCK_STREAM, 0);
  fd.current_status = server_status::Starting;
  if (fd.connection < 0)
    fd.current_status = server_status::CreatingSocket;
  return fd.current_status;
}

server_interface* http_server::create_socket() {

  create_socket_fd(server_socket);

  if (server_socket.current_status != server_status::Starting) {
    std::cerr << "Failed to create server socket" << std::endl;
    server_socket.current_status = server_status::ErrorOrException;
    return this;
  }

  int opt = 1;
  setsockopt(server_socket.connection, SOL_SOCKET, SO_REUSEADDR,
             (const char*)&opt, sizeof(opt));

  sockaddr_in serverAddr{};
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = INADDR_ANY;
  serverAddr.sin_port = htons(server_socket.port);

  if (bind(server_socket.connection, (struct sockaddr*)&serverAddr,
           sizeof(serverAddr)) == SOCKET_ERROR) {
    std::cerr << "Failed to bind server socket: " << std::endl;
    server_socket.current_status = server_status::BindingSocket;
    return (this);
  }

  if (listen(server_socket.connection, MAX_CONNECTIONS) == SOCKET_ERROR) {
    std::cerr << "Failed to listen on server socket" << std::endl;
    server_socket.current_status = server_status::Listening;
    return (this);
  }

  std::cout << "Server v0.1 started\nListening on port : 0.0.0.0:"
            << server_socket.port << " " << std::endl;
  server_socket.current_status = server_status::Running;
  return (this);
}

server_interface* http_server::start() {
  while (server_socket.current_status == server_status::Running) {
    // Accept incoming connection
    sockaddr_in clientAddr{};
    std::shared_ptr<socket_connection> connection(new socket_connection);

    connection->connection =
        accept(server_socket.connection, (struct sockaddr*)&connection->address,
               (socklen_t*)&connection->address_size);
    if (connection->connection == INVALID_SOCKET) {
      std::cerr << "Failed to accept client connection" << std::endl;
      continue;
    } else {
      std::thread clientThread(std::bind(&simple_server::http_server::handle_connection_request, this, connection));
      clientThread.detach();
    }
  }
  closesocket(this->server_socket.connection);
#ifdef WIN32
  // Cleanup WinSock
  WSACleanup();
#endif
  return this;
}

void http_server::handle_connection_request(
    std::shared_ptr<socket_connection> con) {

  constexpr int default_max_buffer= 2048;
  char buffer[default_max_buffer] = {0x00};

  int bytes_read = recv(con->connection, buffer, 1024, 0);

  if (bytes_read > 0) {
    String request(buffer);
    auto http_verb = extract_http_verb(request);

    auto requestVerb = parseRequestVerb(request);
    String urlDynamicPath = extract_url_with_query_params(request);
    String url = extract_url(request);
    auto headers = parse_headers(request);
    StringMap queryParams = parse_url_params(urlDynamicPath);
    String body = extract_body(request);
    auto it = query_endpoint_from_map(url, routes_map);

    String response;
    
    if (it.first == INVALID_HTTP_URI) {
      StringStream body;

      body << "HTTP VERB: " << http_verb << "\n"
           << "URL: " << url << "\n"
           << "Dynamic Url: " << urlDynamicPath << "\n"
           << "HTTP Headers: ";
      for (auto header : headers) {
        body << header.first << ": " << header.second << "\n";
      }
      if (!queryParams.empty()) {
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
      request.query_params = queryParams;
      request.body = body;
      auto res = it.second(&request);
      response = res->buildResponse();
    }
    int querysend =
        send(con->connection, response.c_str(), response.length(), 0);

  }
  closesocket(con->connection);
}

std::pair<String, endpointcb> http_server::query_endpoint_from_map(const String uri, const std::unordered_map<std::string, endpointcb>& routes_map)
{
  std::pair<String, endpointcb> res;
  res.first = std::string(INVALID_HTTP_URI);


  if (uri.size() == 1 && uri == "/") {
    for (auto it : routes_map) {
      if (it.first == "/")
        return it;
    }
    return res;
  }

  auto tryfnd = routes_map.find(uri);
  if(tryfnd!=nullptr){
    res.second = tryfnd->second;
    res.first = tryfnd->first;
    std::cout <<"First=>"<<res.first;
    
    return res;
  }else{
    
    for(auto it : routes_map)
    {  
       std::cout <<"Try regex: "<<it.first<<" On uri: "<<uri<<"\n";
       std::regex pattern(it.first);
       if(std::regex_match(uri, pattern)){
             res.first = it.first;
             res.second = it.second;
             return res;
       } 
    }
  }
  
  auto splittedUri = split_words(uri, '/');
  for (auto it : routes_map) {
    auto eSplitted = split_words(it.first, '/');
    if (eSplitted.size() != splittedUri.size())
      continue;
    bool status = validate_pattern_by_url(splittedUri, eSplitted);
    if (status == true)
      return it;
  }
  return res;
}

http_server *http_server::add_route_handler(std::string url, endpointcb cb){
  routes_map[url] = cb;
  return this;
}

http_server *http_server::set_assets_folder(std::string path){
   if(check_if_path_exist(path)){
    this->root_assets_path = path;
   }else{
    this->root_assets_path = get_current_dir_name()+std::string("/")+"/www";
   }
   return (this);
}

http_server::http_server(short port, std::string id)
    : server_interface(port, id) {}
}  // namespace simple_server

 