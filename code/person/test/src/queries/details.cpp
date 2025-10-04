

#include <memory>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "test/tools/application.hpp"
#include "test/tools/matchers.hpp"
#include "test/tools/redland.hpp"

#include "common/comparators.hpp"
#include "person/queries/details.hpp"

namespace test
{

//  The retrieve_related_persons function tests
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

namespace {

struct Param
{
    const char* case_name;
    const char* data_file;
    const char* proband_uri;

    std::vector<common::Person> expected_partners;
};

class DetailsQueries_RetrievePersonPartners : public ::testing::TestWithParam<Param> {};

TEST_P(DetailsQueries_RetrievePersonPartners, NormalSuccessCases)
{
    const Param& param = GetParam();
    tools::scoped_redland_ctx ctx = tools::initialize_redland_ctx();

    tools::load_rdf(ctx->world, ctx->model, tools::get_program_path() / param.data_file);

    
    const std::vector<common::Person>& expected_partners = param.expected_partners;
    const auto proband = std::make_shared<common::Person>(param.proband_uri);

    std::vector<std::shared_ptr<common::Person>> actual_partners =
        person::retrieve_person_partners(
            proband.get(), ctx->world, ctx->model);

    EXPECT_THAT(
        actual_partners,
        ::testing::UnorderedPointwise(
            test::tools::PointeeLeft(::testing::Eq()),
            expected_partners));
};

const std::vector<Param> g_params {
    {
        "NoPartner",
        "data/deps_queries/retrieve_person_partners/normal_success_cases/model-00_no-partner.ttl",
        "http://example.org/P1",
        {
        }
    },
    {
        "SinglePartner",
        "data/deps_queries/retrieve_person_partners/normal_success_cases/model-01_one-partner.ttl",
        "http://example.org/P1",
        {
            common::Person("http://example.org/P2")
        }
    },
    {
        "SinglePartnerInverted",
        "data/deps_queries/retrieve_person_partners/normal_success_cases/model-01_one-partner.ttl",
        "http://example.org/P2",
        {
            common::Person("http://example.org/P1")
        }
    },
    {
        "TwoPartnersP1",
        "data/deps_queries/retrieve_person_partners/normal_success_cases/"
        "model-02_two-partners.ttl",
        "http://example.org/P1",
        {
            common::Person("http://example.org/P2"),
            common::Person("http://example.org/P3"),
        }
    },
    {
        "TwoPartnersP2",
        "data/deps_queries/retrieve_person_partners/normal_success_cases/"
        "model-02_two-partners.ttl",
        "http://example.org/P2",
        {
            common::Person("http://example.org/P1")
        }
    },
    {
        "TwoPartnersP3",
        "data/deps_queries/retrieve_person_partners/normal_success_cases/"
        "model-02_two-partners.ttl",
        "http://example.org/P3",
        {
            common::Person("http://example.org/P1"),
            common::Person("http://example.org/P4")
        }
    },
    {
        "ThreeGensG1P1",
        "data/deps_queries/retrieve_person_partners/normal_success_cases/"
        "model-03_three-generations.ttl",
        "http://example.org/G1P1",
        {
            common::Person("http://example.org/G1P2"),
        }
    },
    {
        "ThreeGensG2P1",
        "data/deps_queries/retrieve_person_partners/normal_success_cases/"
        "model-03_three-generations.ttl",
        "http://example.org/G2P1",
        {
            common::Person("http://example.org/G2P2"),
        }
    },
    {
        "ThreeGensG3P1",
        "data/deps_queries/retrieve_person_partners/normal_success_cases/"
        "model-03_three-generations.ttl",
        "http://example.org/G3P1",
        {
        }
    }
};

std::string ParamNameGen(const ::testing::TestParamInfo<Param>& info)
{
    return { info.param.case_name };
}


INSTANTIATE_TEST_SUITE_P(
    ,
    DetailsQueries_RetrievePersonPartners,
    ::testing::ValuesIn(g_params),
    ParamNameGen);

} // anonymous namespace

} // namespace test
