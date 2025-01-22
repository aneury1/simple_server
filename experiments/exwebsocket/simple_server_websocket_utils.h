#ifndef SIMPLE_SERVER_WEBSOCKET_ENCODER_DECODER_DEFINED
#define SIMPLE_SERVER_WEBSOCKET_ENCODER_DECODER_DEFINED
#include <string>
#include <vector>
#include <stdint.h>
namespace simple_server{

std::string get_accept_key(const std::string &challenge);
std::vector<uint8_t> hex_2_bytes(const std::string &hex);
std::string base64_encode(const std::string &input);
std::string b64(const std::vector<uint8_t> &data);

}


#endif