#include <string>
#include <utility>

#include <gtest/gtest.h>

#include "common/resource.hpp"

#include "test/tools/assertions.hpp"


struct ExtractResourceParams
{
    common::data_row input_row;
    std::string uri_bn; // URI Binding Name
    std::string expected_uri;
};

class Resource_ExtractResource : public ::testing::TestWithParam<ExtractResourceParams> {};

TEST_P(Resource_ExtractResource, Success)
{
    const ExtractResourceParams& params = GetParam();
    std::shared_ptr<common::Resource> output =
        common::extract_resource<common::Resource>(params.input_row, params.uri_bn);

    EXPECT_EQ(output->get_uri(), boost::urls::url_view(params.expected_uri));
}

const std::vector<ExtractResourceParams> g_success_scenarios_params{
    {
        {
            {"person", "http://example.com/P00123"},
            {"genderType", "http://gedcomx.org/Female"},
            {"birthDate", "1928-06-11"},
            {"deathDate", "1979-11-02"}
        },
        "person",
        {"http://example.com/P00123"}
    },
    {
        {
            {"child", "http://example.com/path/P01001"},
            {"partner", "http://example2.com/P01002"}
        },
        "partner",
        {"http://example2.com/P01002"}
    },
    {
        {
            {"id", "https://whatever.org/P10101"}
        },
        "id",
        {"https://whatever.org/P10101"}
    },
    {
        {
            {"maizi2av", "AMka7!dTE/{WBcp]"},
            {"phe3moal", "http://ieyah8sa.com/vaa7poow"},
            {"xaech8ye", "alc&Dz,n~<,}pX7^[IA8uw)Z4So.&hfZ"},
            {"hei4cok3", "p2d6r1362uie5dbotkz5prk070x5x4gf"}
        },
        "phe3moal",
        {"http://ieyah8sa.com/vaa7poow"}
    }
};

INSTANTIATE_TEST_SUITE_P(
    SuccessScenarios,
    Resource_ExtractResource,
    ::testing::ValuesIn(g_success_scenarios_params));

//  The extract_uri_str_seq function tests
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

namespace suite_extract_uri_str_seq
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

} // namespace suite_extract_uri_str_seq
