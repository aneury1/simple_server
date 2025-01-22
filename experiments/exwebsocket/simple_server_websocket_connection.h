#ifndef SIMPLE_SERVER_WEBSOCKET_CONNECTION_DEFINED
#define SIMPLE_SERVER_WEBSOCKET_CONNECTION_DEFINED
#include <memory>
#include "simple_server/base/simple_server_definitions.h"
#include "simple_server/http/simple_server_http_connection.h"
#include "simple_server/base/simple_server_definitions.h"
#include "simple_server/base/simple_server_thread_container.h"

namespace simple_server{
typedef std::unordered_map<std::string,std::string> StringMap;
enum class websocket_status{
   Ok,
   Error,
   Failed,
   Undefined
};

class websocket_connection{
   public:
   struct socket_connection;
   struct websocket_message{
      std::vector<uint8_t> data;
      std::string str_data;
      std::shared_ptr<socket_connection> connection;
   };
   struct websocket_response{
      std::function<void(const websocket_message& msg)> on_message;
      websocket_response(std::function<void(const websocket_message& msg)> on_msg){
         on_msg = on_message;
      }
   };


   bool valid;
   std::shared_ptr<socket_connection> connection;
   std::function<void(const websocket_message&)> on_message_cb;
   websocket_connection on_message(const websocket_message& message,std::function<void(const websocket_message&)>);
   websocket_connection on_close(std::function<void()> on_cls);

   public:
   void operator()();
   websocket_connection operator=(const websocket_connection& src);
   websocket_connection operator=(std::shared_ptr<socket_connection> src);
   simple_server::websocket_status send_data(std::string message);
};

struct websocket_connection_manager{
 
   std::vector<std::shared_ptr<websocket_connection>> connection_list;
 
   websocket_connection_manager(){}
   public:
      static websocket_connection_manager get(){
         static const websocket_connection_manager manager;
         return manager;
      }
      bool is_websocket_request(const StringMap& request);
      std::shared_ptr<websocket_connection> create_new_ws_connection(int fd);
};

}

#endif