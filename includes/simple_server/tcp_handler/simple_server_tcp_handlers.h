#ifndef SIMPLE_SERVER_TCP_HANDLER
#define SIMPLE_SERVER_TCP_HANDLER
#include "simple_server/base/simple_server_definitions.h"
#include <functional>
#include <memory>
namespace simple_server{

struct tcp_handler{
   virtual void create_server() = 0;
   virtual void accept_new_connection() = 0;
   virtual void on_connection_accepted(std::shared_ptr<socket_connection>) = 0;
   std::function<void(std::shared_ptr<socket_connection>)> connection_accepted_callback;
};

}








#endif