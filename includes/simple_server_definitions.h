#ifndef SIMPLE_SERVER_DEFINITIONS_DEFINED
#define SIMPLE_SERVER_DEFINITIONS_DEFINED
#pragma once



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
#include <sstream>
#include <map>
#include <string>
#include <functional>
#include <algorithm>
#include <unordered_map>
using std::string;

#include "simple_server_http_constant.h"
#include "simple_server_http_connection.h"

namespace simple_server {

#define INVALID_HTTP_URI "INVALID URL"
const int MAX_CONNECTIONS = 1000;
const int INVALID_SOCKET = -1;
const int SOCKET_ERROR = -1;

typedef int socket_address_size;
typedef int socket_type;
typedef std::string String;
typedef std::stringstream StringStream;
typedef std::string String;
typedef std::map<String, String> Parameter;
typedef std::unordered_map<std::string, std::string> StringMap;

enum class server_status {
  NoStartedYet,
  Starting,
  Running,
  CreatingSocket,
  BindingSocket,
  SettingOptions,
  Listening,
  Accepting,
  ErrorOrException
};

enum class supported_protocols_by_default {
  Http,
  RawTcp,
};

struct socket_connection {
  socket_type connection;
  sockaddr_in address;
  socket_address_size address_size;
  server_status current_status;
  short port;
  bool valid;
};

}  // namespace simple_server


#endif