#include "person/error.hpp"

#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace test
{

namespace
{

struct Param
{
    const char* case_name;
    person::person_exception::error_code code;
    const char* details;
    const char* expected;
};

class PersonException_FormatsWhat : public ::testing::TestWithParam<Param> {};

TEST(PersonException_DefaultConstruction, WhatIsJustPrefix)
{
    const person::person_exception ex{};

    EXPECT_STREQ("<uninitialized>", ex.what());
}

TEST_P(PersonException_FormatsWhat, AddsExpectedPrefix)
{
    const Param& param = GetParam();
    const person::person_exception ex(param.code, param.details);

    EXPECT_STREQ(param.expected, ex.what());
}

const std::vector<Param> g_params{
    {
        .case_name = "ResourceNotFound",
        .code = person::person_exception::error_code::resource_not_found,
        .details = "No person resource matched the supplied identifier",
        .expected = "<resource not found> No person resource matched the supplied identifier",
    },
    {
        .case_name = "ResourceNotFoundEmptyMessage",
        .code = person::person_exception::error_code::resource_not_found,
        .details = "",
        .expected = "<resource not found>",
    },
    {
        .case_name = "MultipleResourcesFound",
        .code = person::person_exception::error_code::multiple_resources_found,
        .details =
            "Too Many Resources: found 3 resources when at most one was expected",
        .expected = (
            "<multiple resources found> Too Many Resources: found 3 resources when at most one was"
            " expected"),
    },
    {
        .case_name = "InputContractError",
        .code = person::person_exception::error_code::input_contract_error,
        .details = "Precondition failure: person must not be null",
        .expected = "<input contract error> Precondition failure: person must not be null",
    },
    {
        .case_name = "InternalContractError",
        .code = person::person_exception::error_code::internal_contract_error,
        .details = "Assumption failure: expected good; observed bad",
        .expected = "<internal contract error> Assumption failure: expected good; observed bad",
    }
};

std::string ParamNameGen(const ::testing::TestParamInfo<Param>& info)
{
    return info.param.case_name;
}

INSTANTIATE_TEST_SUITE_P(
    ,
    PersonException_FormatsWhat,
    ::testing::ValuesIn(g_params),
    ParamNameGen);

} // anonymous namespace

} // namespace test

