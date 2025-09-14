#include <string>
#include <utility>

#include <gtest/gtest.h>

#include "common/resource.hpp"

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
    std::shared_ptr<common::Resource> output = common::extract_resource(params.input_row, params.uri_bn);


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
