// Example test case
#include <gtest/gtest.h>
#include <stdint.h>
#include <string>
#include <vector>

const std::string magic_string = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
const std::string defaultTest = "dGhlIHNhbXBsZSBub25jZQ==";
extern std::vector<uint8_t> hexToBytes(const std::string &hex);
extern std::string computeSHA1(const std::string &message);
extern std::string base64Encode(const std::string &input);
extern std::string b64(const std::vector<uint8_t> &data);
extern std::string base64Encode(const void *data, size_t length);
extern std::string getAcceptKey(const std::string &challenge);

TEST(Base64, DefaultExpected) {
  const std::string concatenated = defaultTest + magic_string;
  std::string base64Output = getAcceptKey(defaultTest);

  EXPECT_EQ("s3pPLMBiTxaQ9kYGzzhZRbK+xOo=", base64Output);
  EXPECT_EQ(1, 1);
}

// Main entry point for Google Test
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}