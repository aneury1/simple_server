
#include <gmock/gmock-actions.h>
#include <gmock/gmock-nice-strict.h>
#include <gmock/gmock-spec-builders.h>
#include <gtest/gtest.h>

#include <string>
#include <unordered_map>
#include "variant.h"


TEST(variant_test1, test_maanger1)
{
    Variant<int, double, std::string> var;
    var = 1;
    
    EXPECT_EQ(true, var.is<int>());
}


TEST(variant_test2, test_maanger1)
{
    Variant<int, double, std::string> var;
    var = std::string("1");
    EXPECT_NE(true, var.is<int>());
}