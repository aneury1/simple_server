#include "simple_server_interface.h"
//// Regex to test. 
/// "^\/[^\/]+\.xpp$" /file.xpp == true
/// ^\/.*\.css$ /file/file.css
///             /file.css
#include <gmock/gmock-actions.h>
#include <gmock/gmock-nice-strict.h>
#include <gmock/gmock-spec-builders.h>
#include <gtest/gtest.h>

using namespace testing;

TEST(mocks_test, DISABLED_tESTdISABLE)
{
      ASSERT_EQ(1, 1);
}

TEST(mocks_test, test_if_test_running)
{
      ASSERT_EQ(1, 1);
}