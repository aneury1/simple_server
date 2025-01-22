#ifndef WEBSOCKET_HANDLER_DEFINED
#define WEBSOCKET_HANDLER_DEFINED
#include "defines.h"
#include <set>

struct websocket_manager{
   
   std::set<int> websocket_descriptor;
   
   static websocket_manager* instance;
   static websocket_manager* get();

   bool is_websocket(std::unordered_map<std::string,std::string> headers);
   
   bool is_websocket(int fd);
   
   bool add_new_connection_connection(int fd, std::unordered_map<std::string,std::string> headers, std::string endpoint);
  
   void notify_message_request(int fd);

};



#endif  //WEBSOCKET_HANDLER_DEFINED