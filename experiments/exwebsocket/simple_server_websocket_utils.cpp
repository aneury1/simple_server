
#include "simple_server_websocket_utils.h"
#include "simple_server_websocket_definitions.h"
#include "simple_server_sha1.h"
#include <netinet/in.h>

namespace simple_server{


std::string get_accept_key(const std::string &challenge) {
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




}