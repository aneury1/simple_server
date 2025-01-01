#ifndef SIMPLE_SERVER_HTTP_SERVER_DEFINED
#define SIMPLE_SERVER_HTTP_SERVER_DEFINED
#pragma once
#include <unordered_map>

#include "simple_server_http_connection.h"
#include "simple_server_interface.h"

namespace simple_server{

class http_server : public server_interface {

  public:
    virtual ~http_server() = default;
    virtual server_interface* create_socket() override;
    virtual server_interface* start() override;
    http_server(short port, std::string id);
    http_server *add_route_handler(std::string url, endpointcb);
    http_server *set_assets_folder(std::string path);
  private:
    std::string root_assets_path;
    /// @todo validate if we can work with middleware so we can handle multiples endpointcb for one endpoint.
    ///       by creating an state machine in every request we can do some sub-process of every callback
    ///       configure by especific endpoint.
    std::unordered_map<std::string, endpointcb> routes_map;
    std::pair<String, endpointcb> query_endpoint_from_map(const String uri, const std::unordered_map<std::string, endpointcb>& routes_map);
    void handle_connection_request(std::shared_ptr<socket_connection> con);

  
 

};

}

#endif