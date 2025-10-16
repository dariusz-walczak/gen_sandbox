

#include <algorithm>
#include <memory>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "test/tools/application.hpp"
#include "test/tools/matchers.hpp"
#include "test/tools/printers.hpp"
#include "test/tools/redland.hpp"

#include "common/comparators.hpp"
#include "person/queries/details.hpp"

namespace test
{

namespace
{

struct ComparablePartnerRelation
{
    common::Person partner;
    bool is_inferred;

    friend void PrintTo(const ComparablePartnerRelation& rel, std::ostream* os) {
        *os << "ExpectedPartnerRelation{partner: ";
        PrintTo(rel.partner, os);
        *os << ", is_inferred=" << std::boolalpha << rel.is_inferred << "}";
    };
};

struct ExpectedPartnerRelation
{
    common::Person partner;
    bool is_inferred;

    friend void PrintTo(const ExpectedPartnerRelation& rel, std::ostream* os) {
        *os << "ExpectedPartnerRelation{partner: ";
        PrintTo(rel.partner, os);
        *os << ", is_inferred=" << std::boolalpha << rel.is_inferred << "}";
    };
};

bool operator==(const ComparablePartnerRelation& actual, const ExpectedPartnerRelation& expected)
{
    return ((actual.partner == expected.partner) && (actual.is_inferred == expected.is_inferred));
}

bool operator==(const ExpectedPartnerRelation& expected, const ComparablePartnerRelation& actual)
{
    return ((actual.partner == expected.partner) && (actual.is_inferred == expected.is_inferred));
}

std::vector<ComparablePartnerRelation> adapt(
    const std::vector<common::Person::PartnerRelation>& partners)
{
    std::vector<ComparablePartnerRelation> output;
    output.reserve(partners.size());
    std::transform(
        partners.cbegin(), partners.cend(), std::back_inserter(output),
        [](const auto& rel) { return ComparablePartnerRelation{*rel.partner, rel.is_inferred}; });
    return output;
}

} // anonymous namespace

//  The retrieve_related_persons function tests
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

namespace
{

struct Param
{
    const char* case_name;
    const char* data_file;
    const char* proband_uri;

    std::vector<ExpectedPartnerRelation> expected_partners;
};

class DetailsQueries_RetrievePersonPartners : public ::testing::TestWithParam<Param> {};

TEST_P(DetailsQueries_RetrievePersonPartners, NormalSuccessCases)
{
    const Param& param = GetParam();
    tools::scoped_redland_ctx ctx = tools::initialize_redland_ctx();
    tools::load_rdf(ctx->world, ctx->model, tools::get_program_path() / param.data_file);

    const std::vector<ExpectedPartnerRelation>& expected_partners = param.expected_partners;
    const auto proband = std::make_shared<common::Person>(param.proband_uri);

    const auto& actual_partners = adapt(
        person::retrieve_person_partners(
            proband.get(), ctx->world, ctx->model));

    EXPECT_THAT(
        actual_partners,
        ::testing::UnorderedPointwise(
            ::testing::Eq(),
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
            {common::Person("http://example.org/P2"), false}
        }
    },
    {
        "SinglePartnerInverted",
        "data/deps_queries/retrieve_person_partners/normal_success_cases/model-01_one-partner.ttl",
        "http://example.org/P2",
        {
            {common::Person("http://example.org/P1"), false}
        }
    },
    {
        "TwoPartnersP1",
        "data/deps_queries/retrieve_person_partners/normal_success_cases/"
        "model-02_two-partners.ttl",
        "http://example.org/P1",
        {
            {common::Person("http://example.org/P2"), false},
            {common::Person("http://example.org/P3"), false}
        }
    },
    {
        "TwoPartnersP2",
        "data/deps_queries/retrieve_person_partners/normal_success_cases/"
        "model-02_two-partners.ttl",
        "http://example.org/P2",
        {
            {common::Person("http://example.org/P1"), false}
        }
    },
    {
        "TwoPartnersP3",
        "data/deps_queries/retrieve_person_partners/normal_success_cases/"
        "model-02_two-partners.ttl",
        "http://example.org/P3",
        {
            {common::Person("http://example.org/P1"), false},
            {common::Person("http://example.org/P4"), false}
        }
    },
    {
        "ThreeGensG1P1",
        "data/deps_queries/retrieve_person_partners/normal_success_cases/"
        "model-03_three-generations.ttl",
        "http://example.org/G1P1",
        {
            {common::Person("http://example.org/G1P2"), false}
        }
    },
    {
        "ThreeGensG2P1",
        "data/deps_queries/retrieve_person_partners/normal_success_cases/"
        "model-03_three-generations.ttl",
        "http://example.org/G2P1",
        {
            {common::Person("http://example.org/G2P2"), false}
        }
    },
    {
        "ThreeGensG3P1",
        "data/deps_queries/retrieve_person_partners/normal_success_cases/"
        "model-03_three-generations.ttl",
        "http://example.org/G3P1",
        {
        }
    },
    {
        "OWL84_P00010",
        "data/deps_queries/retrieve_person_partners/normal_success_cases/"
        "model-04_owl-84-repro.ttl",
        "http://example.org/P00010",
        {
            {common::Person("http://example.org/P00000"), false}
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

//  The retrieve_person_father and retrieve_person_mother functions tests
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

namespace suite2
{

struct Param
{
    const char* case_name;
    const char* data_file;
    const char* proband_uri;

    std::optional<common::Person> expected_father;
    std::optional<common::Person> expected_mother;
};

class DetailsQueries_RetrievePersonParents : public ::testing::TestWithParam<Param> {};

TEST_P(DetailsQueries_RetrievePersonParents, NormalSuccessCases)
{
    const Param& param = GetParam();
    tools::scoped_redland_ctx ctx = tools::initialize_redland_ctx();
    tools::load_rdf(ctx->world, ctx->model, tools::get_program_path() / param.data_file);

    const auto proband = std::make_shared<common::Person>(param.proband_uri);

    const auto& actual_father =
        person::retrieve_person_father_opt(proband.get(), ctx->world, ctx->model);
    const auto& actual_mother =
        person::retrieve_person_mother_opt(proband.get(), ctx->world, ctx->model);

    if (param.expected_father.has_value())
    {
        EXPECT_EQ(param.expected_father, *actual_father);
    }

    if (param.expected_mother.has_value())
    {
        EXPECT_EQ(param.expected_mother, *actual_mother);
    }
};

const std::vector<Param> g_params {
    {
        "NoParents",
        "data/deps_queries/retrieve_person_parents/normal_success_cases/model-00_no-parents.ttl",
        "http://example.org/P1",
        {},
        {}
    },
    {
        "FatherOnly",
        "data/deps_queries/retrieve_person_parents/normal_success_cases/model-01_father-only.ttl",
        "http://example.org/P1",
        {common::Person("http://example.org/P2")},
        {}
    },
    {
        "MotherOnly",
        "data/deps_queries/retrieve_person_parents/normal_success_cases/model-02_mother-only.ttl",
        "http://example.org/P1",
        {},
        {common::Person("http://example.org/P3")}
    },
    {
        "BothParents",
        "data/deps_queries/retrieve_person_parents/normal_success_cases/model-03_both-parents.ttl",
        "http://example.org/P1",
        {common::Person("http://example.org/P2")},
        {common::Person("http://example.org/P3")}
    }
};

std::string ParamNameGen(const ::testing::TestParamInfo<Param>& info)
{
    return { info.param.case_name };
}


INSTANTIATE_TEST_SUITE_P(
    ,
    DetailsQueries_RetrievePersonParents,
    ::testing::ValuesIn(g_params),
    ParamNameGen);

} // namespace suite2

} // namespace test
