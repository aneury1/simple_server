#include <errno.h>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <sstream>
#include <stdint.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

extern std::string getAcceptKey(const std::string &challenge);
extern std::vector<uint8_t> hexToBytes(const std::string &hex);
extern std::string computeSHA1(const std::string &message);
extern std::string base64Encode(const std::string &input);
extern std::string b64(const std::vector<uint8_t> &data);
using namespace std;

enum class Opcode : int {
  Text = 0x1,   /// Text frame
  Binary = 0x2, // Binary frame
  Close = 0x8,  //	Connection Close frame
  Ping = 0x9,   ///	Ping frame
  Pong = 0xA    ///	Pong frame
};

// Function to send a WebSocket frame
bool sendWebSocketFrame(int socket_fd, const std::vector<uint8_t> &payload,
                        uint8_t opcode) {
  std::vector<uint8_t> frame;

  // First byte: FIN bit (1) and Opcode
  uint8_t fin = 0x80; // Set FIN to 1 (final frame)
  frame.push_back(fin | opcode);

  // Second byte: Mask bit (0 for server-to-client) and Payload length
  size_t payload_size = payload.size();
  if (payload_size <= 125) {
    frame.push_back(static_cast<uint8_t>(payload_size));
  } else if (payload_size <= 0xFFFF) {
    frame.push_back(126); // Extended payload length: 2 bytes
    frame.push_back((payload_size >> 8) & 0xFF);
    frame.push_back(payload_size & 0xFF);
  } else {
    frame.push_back(127); // Extended payload length: 8 bytes
    for (int i = 7; i >= 0; --i) {
      frame.push_back((payload_size >> (i * 8)) & 0xFF);
    }
  }

  // Append the payload to the frame
  frame.insert(frame.end(), payload.begin(), payload.end());

  // Send the frame over the socket
  ssize_t sent_bytes = send(socket_fd, frame.data(), frame.size(), 0);
  if (sent_bytes == -1) {
    std::cerr << "Error: Failed to send WebSocket frame.\n";
    return false;
  }

  std::cout << "Sent WebSocket frame: " << sent_bytes << " bytes.\n";
  return true;
}

// Function to receive and parse a WebSocket frame
bool receiveWebSocketFrame(int socket_fd) {
  uint8_t header[2];

  // Step 1: Read the first 2 bytes of the WebSocket frame header
  ssize_t bytes_received = recv(socket_fd, header, 2, 0);
  if (bytes_received < 2) {
    std::cerr << "Failed to receive WebSocket frame header.\n";
    return false;
  }

  // Parse the first byte
  bool fin = (header[0] & 0x80) != 0; // FIN bit
  uint8_t opcode = header[0] & 0x0F;  // Opcode

  // Parse the second byte
  bool masked = (header[1] & 0x80) != 0; // MASK bit
  uint64_t payload_length = header[1] & 0x7F;

  // Step 2: Determine the actual payload length
  if (payload_length == 126) { // Extended payload: 2 bytes
    uint8_t extended_length[2];
    recv(socket_fd, extended_length, 2, 0);
    payload_length = (extended_length[0] << 8) | extended_length[1];
  } else if (payload_length == 127) { // Extended payload: 8 bytes
    uint8_t extended_length[8];
    recv(socket_fd, extended_length, 8, 0);
    payload_length = 0;
    for (int i = 0; i < 8; ++i) {
      payload_length = (payload_length << 8) | extended_length[i];
    }
  }

  // Step 3: Read masking key if present
  uint8_t masking_key[4] = {0};
  if (masked) {
    recv(socket_fd, masking_key, 4, 0);
  }

  // Step 4: Read the payload data
  std::vector<uint8_t> payload(payload_length);
  size_t total_received = 0;
  while (total_received < payload_length) {
    ssize_t received = recv(socket_fd, payload.data() + total_received,
                            payload_length - total_received, 0);
    if (received <= 0) {
      std::cerr << "Error reading WebSocket payload.\n";
      return false;
    }
    total_received += received;
  }

  // Step 5: Unmask the payload if needed
  if (masked) {
    for (size_t i = 0; i < payload_length; ++i) {
      payload[i] ^= masking_key[i % 4];
    }
  }

  // Step 6: Handle the frame based on its opcode
  switch (opcode) {
  case 0x1: { // Text frame
    std::string text_message(payload.begin(), payload.end());
    std::cout << "Received Text Frame: " << text_message << "\n";
    break;
  }
  case 0x2: { // Binary frame
    std::cout << "Received Binary Frame: " << payload_length << " bytes\n";
    break;
  }
  case 0x8: { // Close frame
    std::cout << "Received Close Frame\n";
    break;
  }
  case 0x9: { // Ping frame
    std::cout << "Received Ping Frame\n";
    break;
  }
  case 0xA: { // Pong frame
    std::cout << "Received Pong Frame\n";
    break;
  }
  default:
    std::cerr << "Unknown Frame Opcode: " << static_cast<int>(opcode) << "\n";
    return false;
  }

  return true;
}

void receive_data(int socket, unsigned char *buffer, const int length) {
  int vleng = length == -1 ? 4096 : length;
  int rsize = recv(socket, (char *)buffer, vleng, 0);

  if (rsize <= 0)
    buffer[0] = 0x00;
  std::cout << "recv size " << rsize << "\n";
}

void read_line(int socket, std::string &line) {
  char ch;
  line.clear();
  line = "";
  do {
    int r = recv(socket, &ch, 1, 0);
    if (r <= 0) {
      return;
    }
    line += ch;
    if (line.find("\r\n") != std::string::npos) {
      line = line.substr(0, line.length() - 2);
      return;
    }
  } while (1);
}

std::pair<std::string, std::string> split_by(std::string line,
                                             std::string where) {
  std::pair<std::string, std::string> ret;
  ret.first = "NOTHING";
  int pos = line.find(where);
  if (pos != std::string::npos) {
    ret.first = line.substr(0, pos);
    ret.second = line.substr(pos + where.length(), line.length());
  }
  return ret;
}
std::map<std::string, std::string> extract_request(int socke) {
  std::map<std::string, std::string> ret;
  std::string line;
  do {
    read_line(socke, line);

    if (line.length() > 0) {
      /// std::cout <<"line: "<<line<<"\n";
      auto splitted = split_by(line, ": ");
      if (splitted.first == "NOTHING") {
        continue;
      }
      ret[splitted.first] = splitted.second;
      std::cout << "Header: " << splitted.first << "->"
                << " Value: " << splitted.second << "\n";
    }
  } while (!line.empty());
  return ret;
}

bool do_ws_handshake(int client, std::map<std::string, std::string> headers) {
  const std::string magic_string = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
  auto it = headers.find("Sec-WebSocket-Key");
  if (it == headers.end()) {
    std::cerr << "Error: Sec-WebSocket-Key header not found!" << std::endl;
    return false;
  }
  const std::string websocket_key = it->second;
  const std::string base64Output = getAcceptKey(websocket_key);
  std::string accept_key = base64Output;
  // 5. Prepare the HTTP response
  std::string response = "HTTP/1.1 101 Switching Protocols\r\n"
                         "Upgrade: websocket\r\n"
                         "Connection: Upgrade\r\n"
                         "Sec-WebSocket-Accept: " +
                         accept_key + "\r\n\r\n";
  int sent_bytes = send(client, response.c_str(), response.length(), 0);
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

void echo_server(int client, std::map<std::string, std::string> headers) {

  while (1) {
    auto msg = read_frame(client);
    if (msg.length() > 0) {
      std::cout << "msg received";
      send_frame(client, msg);
    } else {
      std::cout << "Not msg";
      break;
    }
  }
}

int main(int argc, char *argv[]) {

  std::string sport = "9000";

  if (argc > 1) {
    sport = argv[1];
  }
  std::cout << "Port to read" << sport << "\n";
  int port = std::stoi(sport.c_str());
  sockaddr_in server_address;
  int server = socket(AF_INET, SOCK_STREAM, 0);

  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(port);
  server_address.sin_family = AF_INET;

  int result =
      bind(server, (sockaddr *)&server_address, sizeof(server_address));

  if (result != 0) {
    std::cout << "Probably this port  is bussy" << errno << "\n";
    return 0;
  }

  int opt = 1;
  result = setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  result = listen(server, 2);

  while (true) {
    int client = accept(server, NULL, NULL);
    std::cout << "Client =" << client << "\n";
    auto headers = extract_request(client);

    if (headers["Sec-WebSocket-Key"].length() > 0) {
      std::cout << "this is websocket\n";
      bool do_hs = do_ws_handshake(client, headers);

      if (do_hs) {
        echo_server(client, headers);
      } else {
        std::cout << "Error on doing hand shake, connection would faild.";
      }

    } else {
      std::cout << "nothing to do\n";
    }

    std::cout << "Finish client reads";
    close(client);
  }
}
