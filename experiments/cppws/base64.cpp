#include <string>

std::string base64Encode(const std::string &input) {
  // Base64 character set
  const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                              "abcdefghijklmnopqrstuvwxyz"
                              "0123456789+/";

  std::string encoded;
  size_t input_length = input.size();
  unsigned char a, b, c; // 3 bytes to process at a time
  int i = 0;

  while (i < input_length) {
    // Read up to 3 bytes at a time
    a = i < input_length ? input[i++] : 0;
    b = i < input_length ? input[i++] : 0;
    c = i < input_length ? input[i++] : 0;

    // Combine the 3 bytes into 4 Base64 characters
    encoded += base64_chars[(a >> 2) & 0x3F];
    encoded += base64_chars[((a << 4) & 0x30) | ((b >> 4) & 0x0F)];
    encoded += (i > input_length + 1)
                   ? '='
                   : base64_chars[((b << 2) & 0x3C) | ((c >> 6) & 0x03)];
    encoded += (i > input_length) ? '=' : base64_chars[c & 0x3F];
  }

  return encoded;
}
