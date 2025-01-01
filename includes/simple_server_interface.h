#ifndef SIMPLE_SERVER_HTTP_INTERFACE_DEFINED
#define SIMPLE_SERVER_HTTP_INTERFACE_DEFINED
#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include "simple_server_definitions.h"

namespace simple_server {

struct server_interface {

  String identifier;
  socket_connection server_socket;
  server_interface(short port, String id){
    server_socket.port = port;
    identifier = id;
  }
  virtual ~server_interface() = default;
  virtual server_interface* create_socket() = 0;
  virtual server_interface* start() = 0;
};

struct default_server_interface : public server_interface{
  default_server_interface(short port, String id): server_interface(port,id){}
  virtual ~default_server_interface() = default;
  virtual server_interface* create_socket() = 0;
  virtual server_interface* start() = 0;
};

}  // namespace simple_server
#endif