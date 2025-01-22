#include <gmock/gmock-actions.h>
#include <gmock/gmock-nice-strict.h>
#include <gmock/gmock-spec-builders.h>
#include <gtest/gtest.h>

#include "string_utils.h"

TEST(replace_all_test, test_maanger1)
{
    std::string upper = "HOLA\r\nPEREZ";
    std::string lower = replace_all(upper,"\r\n"," ");
    
    EXPECT_EQ(lower, "HOLA PEREZ");
}


TEST(lowercase_test1, test_maanger1)
{
    std::string upper = "HOLA";
    std::string lower = to_lower_case(upper);
    
    EXPECT_EQ(lower, "hola");
}

TEST(replace_all_and_lowercase_test, test_maanger1)
{
    std::string upper = "HOLA\r\nPEREZ";
    std::string lower = to_lower_case(replace_all(upper,"\r\n"," "));
    
    EXPECT_EQ(lower, "hola perez");
}