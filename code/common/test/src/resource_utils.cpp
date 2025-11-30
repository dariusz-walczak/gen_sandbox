#include <list>

#include <gtest/gtest.h>

#include "common/common_exception.hpp"
#include "common/resource_utils.hpp"
#include "common/person.hpp"

#include "test/tools/assertions.hpp"

//  The extract_uri_str_seq function tests
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

namespace test::suite_extract_uri_str_seq
{

struct Param
{
    const char* case_name;
    std::vector<std::shared_ptr<common::Resource>> resources;
    std::vector<std::string> expected_uris;
};

std::string ParamNameGen(const ::testing::TestParamInfo<Param>& info)
{
    return { info.param.case_name };
}

class Resource_ExtractUriStrSeq_ValidInput : public ::testing::TestWithParam<Param> {};

TEST_P(Resource_ExtractUriStrSeq_ValidInput, NormalSuccessCases)
{
    const Param& param = GetParam();

    std::vector<std::string> actual_uris = common::extract_uri_str_seq(param.resources);

    EXPECT_EQ(actual_uris, param.expected_uris);
}

const std::vector<Param> g_normal_success_cases_params{
    {
        .case_name="EmptyInput",
        .resources={},
        .expected_uris={}
    },
    {
        .case_name="OneInputResource",
        .resources={
            std::make_shared<common::Resource>("http://example.com/R1")
        },
        .expected_uris={
            "http://example.com/R1"
        }
    },
    {
        .case_name="TwoInputResources",
        .resources={
            std::make_shared<common::Resource>("http://example.com/R1"),
            std::make_shared<common::Resource>("http://alternative.example/uri/1")
        },
        .expected_uris={
            "http://example.com/R1",
            "http://alternative.example/uri/1"
        }
    },
    {
        .case_name="SixInputResourcesUnsorted",
        .resources={
            std::make_shared<common::Resource>("http://example.com/R1"),
            std::make_shared<common::Resource>("http://alternative.example/uri/1"),
            std::make_shared<common::Resource>("https://another.example/X06908513"),
            std::make_shared<common::Resource>("http://example.com/R7"),
            std::make_shared<common::Resource>("http://alternative.example/uri/2"),
            std::make_shared<common::Resource>("http://example.com/P00012"),
        },
        .expected_uris={
            "http://example.com/R1",
            "http://alternative.example/uri/1",
            "https://another.example/X06908513",
            "http://example.com/R7",
            "http://alternative.example/uri/2",
            "http://example.com/P00012"
        }
    }
};

INSTANTIATE_TEST_SUITE_P(
    ,
    Resource_ExtractUriStrSeq_ValidInput,
    ::testing::ValuesIn(g_normal_success_cases_params),
    ParamNameGen);

TEST_F(Resource_ExtractUriStrSeq_ValidInput, ResourceSetSuccessCase)
{
    std::set<std::shared_ptr<common::Resource>> resource_set = {
        std::make_shared<common::Resource>("http://example.com/R1"),
        std::make_shared<common::Resource>("http://example.com/R2"),
        std::make_shared<common::Resource>("http://example.com/R3")
    };

    std::set<std::string> expected_uris = {
        "http://example.com/R1", "http://example.com/R2", "http://example.com/R3"
    };

    std::vector<std::string> actual_uris = common::extract_uri_str_seq(resource_set);
    std::set<std::string> actual_uris_set(actual_uris.begin(), actual_uris.end());

    EXPECT_EQ(actual_uris_set, expected_uris);
}

TEST_F(Resource_ExtractUriStrSeq_ValidInput, PersonListSuccessCase)
{
    std::list<std::shared_ptr<common::Person>> person_list = {
        std::make_shared<common::Person>("http://example.com/P1"),
        std::make_shared<common::Person>("http://example.com/P2"),
        std::make_shared<common::Person>("http://example.com/P3")
    };

    std::vector<std::string> expected_uris = {
        "http://example.com/P1", "http://example.com/P2", "http://example.com/P3"
    };

    std::vector<std::string> actual_uris = common::extract_uri_str_seq(person_list);

    EXPECT_EQ(actual_uris, expected_uris);
}


class Resource_ExtractUriStrSeq_InvalidInput : public ::testing::TestWithParam<Param> {};

TEST_P(Resource_ExtractUriStrSeq_InvalidInput, NullResourceInputContractViolation)
{
    const Param& param = GetParam();

    EXPECT_THROW_WITH_CODE(
        common::extract_uri_str_seq(param.resources),
        common::common_exception, common::common_exception::error_code::input_contract_error);
}

const std::vector<Param> g_null_resource_cases_params{
    {
        .case_name="OneNullPtr",
        .resources={nullptr},
        .expected_uris={}
    },
    {
        .case_name="OneNullPtrTwoNonNullPtrs",
        .resources={
            std::make_shared<common::Resource>("http://example.com/R1"),
            nullptr,
            std::make_shared<common::Resource>("http://example.com/R2")
        },
        .expected_uris={}
    },
    {
        .case_name="TwoNullPtrsTwoNonNullPtrs",
        .resources={
            std::make_shared<common::Resource>("http://example.com/R1"),
            nullptr,
            std::make_shared<common::Resource>("http://example.com/R2"),
            nullptr
        },
        .expected_uris={}
    }
};

INSTANTIATE_TEST_SUITE_P(
    ,
    Resource_ExtractUriStrSeq_InvalidInput,
    ::testing::ValuesIn(g_null_resource_cases_params),
    ParamNameGen);

} // namespace test::suite_extract_uri_str_seq
