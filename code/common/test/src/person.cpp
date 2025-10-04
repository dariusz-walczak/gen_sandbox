#include <gtest/gtest.h>

#include "common/person.hpp"

TEST(Person_GenderToString, Success)
{
    EXPECT_EQ("uninitialized", common::to_string(common::Gender::Uninitialized));
    EXPECT_EQ("male", common::to_string(common::Gender::Male));
    EXPECT_EQ("female", common::to_string(common::Gender::Female));
    EXPECT_EQ("unknown", common::to_string(common::Gender::Unknown));
}
