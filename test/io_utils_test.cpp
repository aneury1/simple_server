#include <gmock/gmock-actions.h>
#include <gmock/gmock-nice-strict.h>
#include <gmock/gmock-spec-builders.h>
#include <gtest/gtest.h>

#include <vector>
#include <string>
#include "io_utils.h"

TEST(list_dir_test, test_maanger1)
{ 
    std::vector<std::string> dir = list_dir("/home/neon/Documents/projects/simple_server/src/");
    EXPECT_EQ(dir.size(), 2);
}
