#include <gtest/gtest.h>

#include "common/resource.hpp"
#include "common/variable.hpp"
#include "common/variable_utils.hpp"

#include "test/tools/assertions.hpp"
#include "test/tools/variable.hpp"

//  The construct_sequence_variable function tests MOVE TO SEPARATE FILE
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

namespace test::suite_construct_sequence_variable
{

struct Param
{
    const char* case_name;
    const char* variable_name;
    std::vector<std::shared_ptr<common::Resource>> resources;
    tools::ComparableVariable expected_variable;
};

std::string ParamNameGen(const ::testing::TestParamInfo<Param>& info)
{
    return { info.param.case_name };
}

class VariableUtils_ConstructSequenceVariable_ValidInput : public ::testing::TestWithParam<Param> {};

TEST_P(VariableUtils_ConstructSequenceVariable_ValidInput, NormalSuccessCases)
{
    const Param& param = GetParam();

    tools::ComparableVariable actual_variable = tools::to_comparable(
        common::construct_sequence_variable(param.variable_name, param.resources));

    EXPECT_EQ(actual_variable, param.expected_variable);
}

const std::vector<Param> g_normal_success_cases_params{
    {
        .case_name="EmptyInput",
        .variable_name="empty_seq_var",
        .resources={},
        .expected_variable={"empty_seq_var", {std::vector<tools::ComparableVariable>{}}}
    },
    {
        .case_name="OneInputResource",
        .variable_name="one_res_var",
        .resources={
            std::make_shared<common::Resource>("http://example.com/R1")
        },
        .expected_variable={
            .name="one_res_var",
            .value={
                std::vector<tools::ComparableVariable>{
                    {"", tools::ComparableResource{.id="http://example.com/R1", .caption=""}}
                }
            }
        }
    },
    {
        .case_name="TwoInputResources",
        .variable_name="two_res_var",
        .resources={
            std::make_shared<common::Resource>("http://example.com/R1"),
            std::make_shared<common::Resource>("http://alternative.example/uri/1")
        },
        .expected_variable={
            .name="two_res_var",
            .value={
                std::vector<tools::ComparableVariable>{
                    {
                        "",
                        tools::ComparableResource{
                            .id="http://example.com/R1",
                            .caption=""
                        }
                    },
                    {
                        "",
                        tools::ComparableResource{
                            .id="http://alternative.example/uri/1", .caption=""}
                    }
                }
            }
        }
    },
    {
        .case_name="SixInputResourcesUnsorted",
        .variable_name="six_res_var",
        .resources={
            std::make_shared<common::Resource>("http://example.com/R1"),
            std::make_shared<common::Resource>("http://alternative.example/uri/1"),
            std::make_shared<common::Resource>("https://another.example/X06908513"),
            std::make_shared<common::Resource>("http://example.com/R7"),
            std::make_shared<common::Resource>("http://alternative.example/uri/2"),
            std::make_shared<common::Resource>("http://example.com/P00012"),
        },
        .expected_variable={
            .name="six_res_var",
            .value={
                std::vector<tools::ComparableVariable>{
                    {
                        "",
                        tools::ComparableResource{
                            .id="http://example.com/R1",
                            .caption=""
                        }
                    },
                    {
                        "",
                        tools::ComparableResource{
                            .id="http://alternative.example/uri/1",
                            .caption=""
                        }
                    },
                    {
                        "",
                        tools::ComparableResource{
                            .id="https://another.example/X06908513",
                            .caption=""
                        }
                    },
                    {
                        "",
                        tools::ComparableResource{
                            .id="http://example.com/R7",
                            .caption=""
                        }
                    },
                    {
                        "",
                        tools::ComparableResource{
                            .id="http://alternative.example/uri/2",
                            .caption=""
                        }
                    },
                    {
                        "",
                        tools::ComparableResource{
                            .id="http://example.com/P00012",
                            .caption=""
                        }
                    }
                }
            }
        }
    }
};

INSTANTIATE_TEST_SUITE_P(
    ,
    VariableUtils_ConstructSequenceVariable_ValidInput,
    ::testing::ValuesIn(g_normal_success_cases_params),
    ParamNameGen);

/** Check if the construct_sequence_variable function raises an input contract error exception when
 *   the resources list contains one or more null pointers. */
class VariableUtils_ConstructSequenceVariable_InvalidInput : public ::testing::TestWithParam<Param> {};

TEST_P(VariableUtils_ConstructSequenceVariable_InvalidInput, NullResourceInputContractViolation)
{
    const Param& param = GetParam();

    EXPECT_THROW_WITH_CODE(
        common::construct_sequence_variable(param.variable_name, param.resources),
        common::common_exception, common::common_exception::error_code::input_contract_error);
}

const std::vector<Param> g_null_resource_cases_params{
    {
        .case_name="OneNullPtrTwoNonNullPtrs",
        .variable_name="one_null_ptr",
        .resources={
            std::make_shared<common::Resource>("http://example.com/R1"),
            nullptr,
            std::make_shared<common::Resource>("http://example.com/R2")
        },
        .expected_variable={}
    },
    {
        .case_name="TwoNullPtrsTwoNonNullPtrs",
        .variable_name="two_null_ptrs",
        .resources={
            nullptr,
            std::make_shared<common::Resource>("http://example.com/R1"),
            std::make_shared<common::Resource>("http://example.com/R2"),
            nullptr
        },
        .expected_variable={}
    }
};

INSTANTIATE_TEST_SUITE_P(
    ,
    VariableUtils_ConstructSequenceVariable_InvalidInput,
    ::testing::ValuesIn(g_null_resource_cases_params),
    ParamNameGen);

} // namespace test::suite_construct_sequence_variable

