#ifndef SIMPLE_SERVER_WEBSOCKET_CONNECTION
#define SIMPLE_SERVER_WEBSOCKET_CONNECTION
#include <string>
#include <unordered_map>
#include <functional>
#include <stdint.h>
#include <vector>
#include <mutex>

namespace simple_server{


struct websocket_message{
   std::vector<uint8_t> payload;
   int type_of_message;
   int connection;
   std::string toString();
};


class websocket_connection;

std::string find_websocket_key(std::unordered_map<std::string,std::string>headers);

class websocket_connection_manager{

    std::function<void(websocket_message)> onmessage_cb;
  
    static std::vector<websocket_connection> connections;
    static std::mutex mut;
    websocket_connection_manager();

public:

    static websocket_connection_manager get(){
        static websocket_connection_manager object;
        return object;
    }
    
    std::string create_new_ws_connection(std::unordered_map<std::string,std::string>headers, int connection);

    websocket_connection_manager on_message(std::function<void(websocket_message)> onmessage);

    bool is_websocket_request(const std::unordered_map<std::string,std::string>& headers);
 

    ///websocket_connection_manager send_message(int id, std::shared_ptr<websocket_message> msg);
};



}


#endif