#include "simple_server/websocket/simple_server_websocket_connection.h"
#include "simple_server/websocket/simple_server_websocket_utils.h"
#include "simple_server/http/simple_server_http_constant.h"
#include "simple_server/base/simple_server_utils.h"

#include <thread>
#include <iostream>

namespace simple_server{


bool websocket_connection_manager::is_websocket_request(const StringMap & request){
  auto it = request.find(http_header_to_string(http_header::Sec_WebSocket_Key));
  if(it!=request.end())
     return true;
  return false;
}

std::shared_ptr<websocket_connection> websocket_connection_manager::create_new_ws_connection(int con){
    std::shared_ptr<websocket_connection> ret(new websocket_connection);
    std::cout <<"weboscket connection thread fd: "<<con<<"\n";
    ////ret->connection = con;
    ///ret->on_close = nullptr; /// Not ipm
    ///ret->on_message_cb = con.on_message_cb;
    std::thread th(*ret);
    th.detach();
    return ret;
}


bool do_ws_handshake(socket_connection client, const Request &request) {
  ///const std::string magic_string = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
  auto it = request.headers.find("Sec-WebSocket-Key");
  if (it == request.headers.end()) {
    std::cerr << "Error: Sec-WebSocket-Key header not found!" << std::endl;
    return false;
  }
  const std::string websocket_key = it->second;
  const std::string base64Output = get_accept_key(websocket_key);
  std::string accept_key = base64Output;
  // 5. Prepare the HTTP response
  std::string response = "HTTP/1.1 101 Switching Protocols\r\n"
                         "Upgrade: websocket\r\n"
                         "Connection: Upgrade\r\n"
                         "Sec-WebSocket-Accept: " +
                         accept_key + "\r\n\r\n";
  int sent_bytes = send_buffer(client, response);//send(client, response.c_str(), response.length(), 0);
  if (sent_bytes < 0) {
    std::cerr << "Error: Failed to send handshake response!" << std::endl;
    return false;
  }
  std::cout << response << "\n\n\n";
  std::cout << "Handshake successful. Response sent (" << sent_bytes
            << " bytes)." << std::endl;
  return true;
}

void send_frame(int client_socket, std::string message) {
  uint8_t frame_header = 0x81; // FIN = 1, Opcode = 0x1 (Text)
  size_t payload_length = message.size();

  // Frame the payload length
  std::vector<uint8_t> frame;
  frame.push_back(frame_header); // FIN + Opcode
  if (payload_length <= 125) {
    frame.push_back(payload_length); // Payload length
  } else if (payload_length <= 0xFFFF) {
    frame.push_back(126); // Extended payload length (2 bytes)
    frame.push_back((payload_length >> 8) & 0xFF);
    frame.push_back(payload_length & 0xFF);
  } else {
    frame.push_back(127); // Extended payload length (8 bytes)
    for (int i = 7; i >= 0; --i) {
      frame.push_back((payload_length >> (i * 8)) & 0xFF);
    }
  }

  // Copy the payload to the frame
  frame.insert(frame.end(), message.begin(), message.end());

  // Send the frame
  send(client_socket, frame.data(), frame.size(), 0);
}


std::string read_frame(int client_socket) {
  uint8_t header[2];

  int d;
  if ((d = recv(client_socket, header, 2, 0)) != 2) {
    std::cerr << "Failed to read frame header." << d << " " << client_socket
              << std::endl;
    return "";
  }

  // Parse the first byte
  bool fin = (header[0] >> 7) & 0x01; // FIN bit
  uint8_t opcode = header[0] & 0x0F;  // Opcode (text, binary, close, etc.)

  // Parse the second byte
  bool masked = (header[1] >> 7) & 0x01; // Mask bit
  uint64_t payload_length = header[1] & 0x7F;
  std::cout << "Payload length: " << payload_length << "\n";
  // Handle extended payload lengths
  if (payload_length == 126) {
    uint8_t extended_payload[2];
    if (recv(client_socket, extended_payload, 2, 0) != 2) {
      std::cerr << "Failed to read extended payload length." << std::endl;
      return "";
    }
    payload_length = (extended_payload[0] << 8) | extended_payload[1];
  } else if (payload_length == 127) {
    uint8_t extended_payload[8];
    if (recv(client_socket, extended_payload, 8, 0) != 8) {
      std::cerr << "Failed to read extended payload length (64-bit)."
                << std::endl;
      return "";
    }
    payload_length = 0;
    for (int i = 0; i < 8; ++i) {
      payload_length = (payload_length << 8) | extended_payload[i];
    }
  }

  // Read masking key if payload is masked
  std::vector<uint8_t> masking_key(4);
  if (masked) {
    if (recv(client_socket, masking_key.data(), 4, 0) != 4) {
      std::cerr << "Failed to read masking key." << std::endl;
      return "";
    }
  }

  // Read the payload data
  std::vector<uint8_t> payload(payload_length);
  uint64_t bytes_read = 0;
  while (bytes_read < payload_length) {
    ssize_t result = recv(client_socket, payload.data() + bytes_read,
                          payload_length - bytes_read, 0);
    if (result <= 0) {
      std::cerr << "Failed to read payload data." << std::endl;
      return "";
    }
    bytes_read += result;
  }

  // Unmask the payload if needed
  if (masked) {
    for (uint64_t i = 0; i < payload_length; ++i) {
      payload[i] ^= masking_key[i % 4];
    }
  }

  // Convert the payload to a string (assuming text frame)
  if (opcode == 0x1) { // Text frame
    return std::string(payload.begin(), payload.end());
  } else {
    std::cerr << "Unsupported opcode: " << static_cast<int>(opcode)
              << std::endl;
    return "";
  }
}

std::vector<uint8_t> read_frame(uint32_t con){
   int fd = con;
   string msg = read_frame(fd);
   std::vector<uint8_t> ret;
   ret.reserve(msg.size());
   std::transform(msg.begin(),msg.end(), ret.begin(), [](char bf){
    return (unsigned char)bf;
   });
   return ret;
}


websocket_connection websocket_connection::on_message(const websocket_message& message,std::function<void(const websocket_message&)> cb){
  ///do some validation....
  if(valid)  
   cb(message);
}

websocket_connection websocket_connection::on_close(std::function<void()> on_cls){
   valid = false;
   /// todo understand the behaviour of close.
}
   
   websocket_connection websocket_connection::operator=(const websocket_connection& src){
       
       /*connection->address = src.connection->address;
       connection->address_size = src.connection->address_size;
       connection->connection = src.connection->connection;
       connection->current_status = src.connection->current_status;
       connection->port = src.connection->port;
       connection->valid = src.connection->valid;
       valid = src.valid;*/
       return *this;
   }

   websocket_connection websocket_connection::operator=(std::shared_ptr<socket_connection> src){
      
      /* connection->address =  src->address;
       connection->address_size = src->address_size;
       connection->connection = src->connection;
       connection->current_status =  src->current_status;
       connection->port =  src->port;
       connection->valid = src->valid;
       valid = connection->valid;*/
       return *this;
}

void websocket_connection::operator()(){
   while(valid){
    std::vector<uint8_t> msg = read_frame((uint32_t)0);
    if (msg.size() > 0) {
      std::cout << "msg received";
      websocket_message message;
      message.connection = connection;
      message.data =msg;
      this->on_message(message, this->on_message_cb);
      ///send_frame(connection, msg);
    } else {
      std::cout << "Not msg";
      break;
    }
   }
}

 websocket_status websocket_connection::send_data(std::string message){
   
   return websocket_status::Failed;
 }


}