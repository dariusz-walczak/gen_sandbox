#include <string>
#include <vector>
#include <optional>
//#include <utility>

#include <gtest/gtest.h>

#include "common/string.hpp"


//  The join function tests
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

namespace suite_join
{

struct Param
{
    const char* case_name;
    std::vector<std::string> range;
    std::optional<std::string> separator;
    std::string expected_string;
};

class String_Join : public ::testing::TestWithParam<Param> {};

TEST_P(String_Join, NormalSuccessCases)
{
    const Param& param = GetParam();

    std::string actual_string;

    if (param.separator.has_value())
    {
        actual_string = common::join(param.range, param.separator.value());
    }
    else
    {
        actual_string = common::join(param.range);
    }

    EXPECT_EQ(actual_string, param.expected_string);
}

const std::vector<Param> g_normal_success_cases_params{
    {
        .case_name="EmptyRangeDefaultSeparator",
        .range={},
        .separator={},
        .expected_string=""
    },
    {
        .case_name="EmptyRangeCustomSeparator",
        .range={},
        .separator={"/"},
        .expected_string=""
    },
    {
        .case_name="SingleItemDefaultSeparator",
        .range={"item"},
        .separator={},
        .expected_string="item"
    },
    {
        .case_name="SingleItemCustomSeparator",
        .range={"item"},
        .separator={"/"},
        .expected_string="item"
    },
    {
        .case_name="TwoItemsDefaultSeparator",
        .range={"item1", "item2"},
        .separator={},
        .expected_string="item1, item2"
    },
    {
        .case_name="TwoItemsCustomSeparator",
        .range={"item1", "item2"},
        .separator={"/"},
        .expected_string="item1/item2"
    },
    {
        .case_name="FiveItemsDefaultSeparator",
        .range={"item1", "item2", "item3", "item4", "item5"},
        .separator={},
        .expected_string="item1, item2, item3, item4, item5"
    }
};

std::string ParamNameGen(const ::testing::TestParamInfo<Param>& info)
{
    return { info.param.case_name };
}

INSTANTIATE_TEST_SUITE_P(
    ,
    String_Join,
    ::testing::ValuesIn(g_normal_success_cases_params),
    ParamNameGen);

} // namespace suite_join
