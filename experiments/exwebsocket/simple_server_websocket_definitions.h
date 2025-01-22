#ifndef SIMPLE_SERVER_WEBSOCKET_DEFINITIONS
#define SIMPLE_SERVER_WEBSOCKET_DEFINITIONS
#include <string>

namespace simple_server{

const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    
const std::string magic_string = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

enum class Opcode : int {
  Text   = 0x1,   /// Text frame
  Binary = 0x2, // Binary frame
  Close  = 0x8,  //	Connection Close frame
  Ping   = 0x9,   ///	Ping frame
  Pong   = 0xA    ///	Pong frame
};


}


#endif