
//// Regex to test. 
/// "^\/[^\/]+\.xpp$" /file.xpp == true
/// ^\/.*\.css$ /file/file.css
///             /file.css
#include <gmock/gmock-actions.h>
#include <gmock/gmock-nice-strict.h>
#include <gmock/gmock-spec-builders.h>
#include <gtest/gtest.h>

#include <unordered_map>

#include "simple_server/websocket/simple_server_websocket_connection.h"
#include "simple_server/base/simple_server_utils.h"

using namespace testing;

TEST(mocks_test, DISABLED_tESTdISABLE)
{
      ASSERT_EQ(1, 1);
}
TEST(websocket_test, test_transform_lowercase_correctly)
{
   std::string uppercase ="UpperCase";
   auto r= simple_server::to_lower_case_str(uppercase);
   EXPECT_EQ(r, std::string("uppercase"));

}

TEST(websocket_test0, test_if_keyfound_or_no_correctly)
{
     std::unordered_map<std::string, std::string>headers;
     headers["Sec-WebSocket-Key"] ="key1";
     auto it1 = simple_server::find_websocket_key(headers);
     EXPECT_EQ(it1, std::string("key1"));
}

TEST(websocket_test1, test_maanger1)
{
     std::unordered_map<std::string, std::string>headers;
     headers["Sec-WebSocket-Key"] ="key1";
     auto it1 = simple_server::websocket_connection_manager::get().is_websocket_request(headers);
     EXPECT_EQ(true, it1);
}


TEST(websocket_test2, test_maanger1)
{
     std::unordered_map<std::string, std::string>headers;
     headers["Sec-WebSocket-Key"] ="key1";
     auto it1 = simple_server::websocket_connection_manager::get().is_websocket_request(headers);
     EXPECT_NE(false, it1);
}