#include "simple_server/base/simple_server_sha1.h"
#include "simple_server/base/simple_server_utils.h"
#include "simple_server/websocket/simple_server_websocket_connection.h"
#include <iostream>
#include <thread>
#include <atomic>
#include <netinet/in.h>
#include <poll.h>
#include <fcntl.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <mutex>
namespace simple_server{

#define MAX_CONNECTIONS 1000


// Base64 encoding table
const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const std::string magic_string = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

std::atomic<int> general_counter=0;

std::string websocket_message::toString(){
   std::string ret;
   for(auto it : payload){
      if((unsigned int)it<30)
         ret +=" ";
      else
         ret += (char)it;
   }
   return ret;
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

struct websocket_connection{
    int fd;
    int id;
    int counter;
    bool valid;
    std::function<void(const websocket_message&)>on_message_received;
    websocket_connection(int fd){
        counter = ++general_counter;
        this->fd = fd;
        valid =true;
        id = counter;
    }
    websocket_connection(websocket_connection& other) = default;
    websocket_connection(websocket_connection&& other) = default;
    void operator=(websocket_connection& other){
      this->fd = other.fd;
      this->counter = other.counter;
      this->valid = other.valid;
      this->on_message_received = other.on_message_received;
    }
    void operator=(websocket_connection&& other){
      this->fd = other.fd;
      this->counter = other.counter;
      this->valid = other.valid;
      this->on_message_received = other.on_message_received;
      other.fd = other.valid = other.counter =0;
      other.on_message_received = nullptr;
    }
    void operator()(){
    ///while(valid){
      std::vector<uint8_t> msg = read_frame((uint32_t)fd);
      if (msg.size() > 0) {
        std::cout << "msg received";
        websocket_message message;
        message.connection = fd;
        message.payload =msg;
        if(on_message_received){
          on_message_received(message);
        }else{
          std::cout <<"sending frame";
          send_frame(fd , message.toString());
        }
      } else {
        std::cout << "Not msg";
      ///  break;
      }
    ///}
    }
    void operator()(websocket_message message){
    while(valid){
      std::vector<uint8_t> msg = read_frame((uint32_t)fd);
      if (msg.size() > 0) {
        std::cout << "msg received";
        if(on_message_received){
          on_message_received(message);
        }else{
          std::cout <<"sending frame";
          send_frame(fd , message.toString());
        }
      } else {
        std::cout << "Not msg";
        break;
      }
    }
    }
};

namespace{
int event_fd;  
bool run_ws_handler = true;
std::vector<websocket_connection> web_sockect_connections;
std::vector<pollfd> poll_fds;
std::mutex mu;

void add_socket_to_the_list(int fd){
  std::lock_guard<std::mutex>m(mu);
  pollfd client_poll_fd = {fd, POLLIN, 0};
  poll_fds.push_back(client_poll_fd);
  web_sockect_connections.emplace_back(websocket_connection(fd));
  std::cout<<"New Descritor:"<< fd<<" Adding new socket to the list:"<< poll_fds.size()<<"\n";

  uint64_t signal = 1;
  write(event_fd, &signal, sizeof(signal));
}

void check_and_notify_events(){
      while (run_ws_handler) {
      ///std::cout <<"Start activity on this thread...size"<< poll_fds.size()<<"\n";
      int activity = poll(poll_fds.data(), poll_fds.size(), -1); // Infinite timeout
      if (activity < 0) {
        std::cout <<"error on activity";
        perror("poll error");
        break;
      }else{
         
      }
      for (size_t i = 0; i < poll_fds.size(); i++) {
        
        if(poll_fds[i].revents & POLLIN){
           
           for(auto it : web_sockect_connections)
           {
            std::cout  
             <<"\nEvent Master: "<<event_fd<<" "
             << " iter="<<i<<" fd "<<" websocket fd: "<< it.fd<<" poll_fds[i].fd:"<< poll_fds[i].fd <<" POLLIN EVENT: "<<bool(poll_fds[i].revents & POLLIN) <<"\n";
              
               
               bool is_master = poll_fds[i].fd == event_fd;
               
               if(it.fd == poll_fds[i].fd){
                std::cout <<"\nstarting reading";
                char buffer[1024];
                ssize_t bytes_read = read(poll_fds[i].fd, buffer, sizeof(buffer));
                buffer[bytes_read+1]=0;
                std::cout << "\nbytes read: "<< bytes_read<<" \nbuffer:"<<buffer<<"\n";
                 if(it.on_message_received){
                    std::cout <<"message received callback\n";
                     it();
                 }else{
                  std::cout <<"Probably this message would be handle sorry\n";
                 }
               }
           }
        }else if(poll_fds[i].revents & POLLERR){
            std::cout <<"error on the descriptor\n";
        }else if(poll_fds[i].revents & POLLHUP){
            std::cout <<"connection close on the descriptor\n";
            close(poll_fds[i].fd);
            poll_fds.erase(poll_fds.begin() + i);
            i--;  // Adjust index after removal
        }

      }
    }
}

}


std::string base64_encode(const void *data, size_t length) {
  std::string output;
  const auto dataPtr = reinterpret_cast<const uint8_t *>(data);
  for (auto i = 0u; i < length; i += 3) {
    const auto bytesLeft = length - i;
    const auto b0 = dataPtr[i];
    const auto b1 = bytesLeft > 1 ? dataPtr[i + 1] : 0;
    const auto b2 = bytesLeft > 2 ? dataPtr[i + 2] : 0;
    output.push_back(base64_chars[b0 >> 2]);
    output.push_back(base64_chars[((b0 & 0x03) << 4) | ((b1 & 0xf0) >> 4)]);
    output.push_back(
        (bytesLeft > 1 ? base64_chars[((b1 & 0x0f) << 2) | ((b2 & 0xc0) >> 6)]
                       : '='));
    output.push_back((bytesLeft > 2 ? base64_chars[b2 & 0x3f] : '='));
  }
  return output;
}

std::vector<uint8_t> hex_2_bytes(const std::string &hex)
{
  std::vector<uint8_t> bytes;
  for (size_t i = 0; i < hex.length(); i += 2) {
    std::string byteString = hex.substr(i, 2);
    uint8_t byte = (uint8_t)strtol(byteString.c_str(), nullptr, 16);
    bytes.push_back(byte);
  }
  return bytes;
}

std::string b64(const std::vector<uint8_t> &data) {
  std::string result;
  size_t i = 0;
  uint8_t byte_array_3[3];
  uint8_t byte_array_4[4];

  for (size_t idx = 0; idx < data.size(); ++idx) {
    byte_array_3[i++] = data[idx];
    if (i == 3) {
      byte_array_4[0] = (byte_array_3[0] & 0xfc) >> 2;
      byte_array_4[1] =
          ((byte_array_3[0] & 0x03) << 4) + ((byte_array_3[1] & 0xf0) >> 4);
      byte_array_4[2] =
          ((byte_array_3[1] & 0x0f) << 2) + ((byte_array_3[2] & 0xc0) >> 6);
      byte_array_4[3] = byte_array_3[2] & 0x3f;

      for (i = 0; i < 4; ++i)
        result += base64_chars[byte_array_4[i]];

      i = 0;
    }
  }

  if (i) {
    for (size_t j = i; j < 3; ++j)
      byte_array_3[j] = '\0';

    byte_array_4[0] = (byte_array_3[0] & 0xfc) >> 2;
    byte_array_4[1] =
        ((byte_array_3[0] & 0x03) << 4) + ((byte_array_3[1] & 0xf0) >> 4);
    byte_array_4[2] =
        ((byte_array_3[1] & 0x0f) << 2) + ((byte_array_3[2] & 0xc0) >> 6);
    byte_array_4[3] = byte_array_3[2] & 0x3f;

    for (size_t j = 0; j < i + 1; ++j)
      result += base64_chars[byte_array_4[j]];

    while (result.size() % 4)
      result += '=';
  }

  return result;
}

std::string get_accept_key(const std::string& challenge){
  auto fullString = challenge + magic_string;
  SHA1 hasher;
  hasher.Input(fullString.c_str(),
               static_cast<unsigned int>(fullString.size()));
  unsigned hash[5];
  hasher.Result(hash);
  for (unsigned int &i : hash) {
    i = htonl(i);
  }
  return base64_encode(hash, sizeof(hash));
}

std::string find_websocket_key(std::unordered_map<std::string,std::string>headers){
    for(auto it : headers){
        auto low_it = to_lower_case_str(it.first);
        if(low_it=="sec-websocket-key")
           return it.second;
    }
    return std::string();
}

websocket_connection_manager::websocket_connection_manager(){
  
    event_fd = eventfd(0, EFD_NONBLOCK);
    if (event_fd == -1) {
        perror("Eventfd creation failed");
        return ;
    }
   pollfd event_poll_fd = {event_fd, POLLIN, 0};
   poll_fds.push_back(event_poll_fd);
 
   std::thread th(check_and_notify_events);
   th.detach();
}

std::string websocket_connection_manager::create_new_ws_connection(std::unordered_map<std::string,std::string>headers,
int connection){
  ///const std::string magic_string = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
  auto it = find_websocket_key(headers);///headers.find("Sec-WebSocket-Key");
  if (it.empty()) {
    std::cerr << "Error: Sec-WebSocket-Key header not found!" << std::endl;
    return  std::string();
  }
  const std::string websocket_key = it;//->second;
  const std::string base64Output = get_accept_key(websocket_key);
  std::string accept_key = base64Output;
 /// websocket_connection ws_connection(connection);
  std::string response_transformed = "HTTP/1.1 101 Switching Protocols\r\n"
                         "Upgrade: websocket\r\n"
                         "Connection: Upgrade\r\n"
                         ////"Content-Security-Policy: script-src 'self' 'unsafe-inline';\r\n"
                         "Sec-WebSocket-Accept: " +
                         accept_key + "\r\n\r\n";
  
  std::cout <<"\nnew connection arrive fd: "<< connection<<"\n";
  fcntl(connection, F_SETFL, O_NONBLOCK);
  add_socket_to_the_list(connection);
  return response_transformed;
}

    
websocket_connection_manager websocket_connection_manager::on_message(std::function<void(websocket_message)> onmessage){
  onmessage_cb= onmessage;
  return (*this);
}

 bool websocket_connection_manager::is_websocket_request(const std::unordered_map<std::string,std::string>& headers){
    auto it = find_websocket_key(headers);
  
    return it.empty()==false;
 }
}
