#include <set>
#include <string>
#include <vector>

#include <gtest/gtest.h>
#include <boost/url.hpp>

#include "test/tools/application.hpp"
#include "test/tools/redland.hpp"

#include "person/queries/deps.hpp"

namespace test
{

//  The retrieve_person_iris function tests
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

namespace {

struct RetrievePersonIrisParams
{
    struct triple
    {
        const char* subject;
        const char* predicate;
        const char* object;
    };

    std::vector<triple> triples;
    std::set<std::string> iris;
};

class DepsQueries_RetrievePersonIris : public ::testing::TestWithParam<RetrievePersonIrisParams> {};

TEST_P(DepsQueries_RetrievePersonIris, NormalSuccessCases)
{
    const RetrievePersonIrisParams& param = GetParam();
    test::tools::scoped_redland_ctx ctx = test::tools::initialize_redland_ctx();

    for (const auto& triple : param.triples)
    {
        test::tools::insert_uuu_statement(
            ctx->world, ctx->model,
            triple.subject, triple.predicate, triple.object);
    }

    common::resource_set resources = person::retrieve_person_iris(ctx->world, ctx->model);

    const std::set<boost::urls::url_view> expected_iris{ param.iris.begin(), param.iris.end() };
    std::set<boost::urls::url_view> actual_iris;

    std::transform(
        resources.begin(), resources.end(), std::inserter(actual_iris, actual_iris.end()),
        [](auto const& r) { return r->get_iri(); });

    EXPECT_EQ(expected_iris, actual_iris);
}

const std::vector<RetrievePersonIrisParams> g_expected_success_params {
    // Case 0: Empty model
    {
        {
        },
        {
        }
    },
    // Case 1: Single person triple
    {
        {
            {
                "http://example.com/P00001",
                "http://www.w3.org/1999/02/22-rdf-syntax-ns#type",
                "http://gedcomx.org/Person"
            }
        },
        {
            "http://example.com/P00001"
        }
    },
    // Case 2: Two person triples
    {
        {
            {
                "http://example.com/P00011",
                "http://www.w3.org/1999/02/22-rdf-syntax-ns#type",
                "http://gedcomx.org/Person"
            },
            {
                "http://example.com/P00012",
                "http://www.w3.org/1999/02/22-rdf-syntax-ns#type",
                "http://gedcomx.org/Person"
            }
        },
        {
            { "http://example.com/P00011" },
            { "http://example.com/P00012" }
        }
    },
    // Case 3: Single non-person triple
    {
        {
            {
                "http://example.com/R00021",
                "http://www.w3.org/1999/02/22-rdf-syntax-ns#type",
                "http://gedcomx.org/Relationship"
            }
        },
        {
        }
    },
    // Case 4: Two person and some non-person triples
    {
        {
            {
                "http://example.com/R00031",
                "http://www.w3.org/1999/02/22-rdf-syntax-ns#type",
                "http://gedcomx.org/Relationship"
            },
            {
                "http://example.com/R00031",
                "http://gedcomx.org/type"
                "http://gedcomx.org/Couple"
            },
            {
                "http://example.com/R00031",
                "http://gedcomx.org/person1"
                "http://example.com/P00032"
            },
            {
                "http://example.com/R00031",
                "http://gedcomx.org/person2"
                "http://example.com/P00042"
            },
            {
                "http://example.com/P00032",
                "http://www.w3.org/1999/02/22-rdf-syntax-ns#type",
                "http://gedcomx.org/Person"
            },
            {
                "http://example.com/P00042",
                "http://www.w3.org/1999/02/22-rdf-syntax-ns#type",
                "http://gedcomx.org/Person"
            }
        },
        {
            "http://example.com/P00032",
            "http://example.com/P00042"
        }
    }
};

INSTANTIATE_TEST_SUITE_P(
    ,
    DepsQueries_RetrievePersonIris,
    ::testing::ValuesIn(g_expected_success_params));

} // anonymous namespace


//  The retrieve_related_persons function tests
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

namespace {

struct Param
{
    const char* case_name;
    const char* data_file;

    std::set<std::pair<std::string, std::string>> expected_pairs;
};

class DepsQueries_RetrieveRelatedPersons : public ::testing::TestWithParam<Param> {};

TEST_P(DepsQueries_RetrieveRelatedPersons, NormalSuccessCases)
{
    const Param& param = GetParam();
    tools::scoped_redland_ctx ctx = tools::initialize_redland_ctx();

    tools::load_rdf(ctx->world, ctx->model, tools::get_program_path() / param.data_file);

    common::data_table pair_table = person::retrieve_related_persons(ctx->world, ctx->model);

    std::set<std::pair<std::string, std::string>> actual_pairs;

    std::transform(
        pair_table.begin(), pair_table.end(),
        std::inserter(actual_pairs, actual_pairs.end()),
        [](auto const& r) { return std::make_pair(r.at("person1"), r.at("person2")); });

    EXPECT_EQ(param.expected_pairs, actual_pairs);
};

const std::vector<Param> g_params {
    {
        "SingleRelation",
        "data/deps_queries/retrieve_related_persons/normal_success_cases/case0.ttl",
        {
            { "http://example.org/P1", "http://example.org/P2" }
        }
    },
    {
        "SingleFamilyPlusOneUnrelated",
        "data/deps_queries/retrieve_related_persons/normal_success_cases/case1.ttl",
        {
            { "http://example.org/P1", "http://example.org/P2" },
            { "http://example.org/P1", "http://example.org/P3" },
            { "http://example.org/P2", "http://example.org/P3" }
        }
    },
    {
        "DuplicateRelation",
        "data/deps_queries/retrieve_related_persons/normal_success_cases/case2.ttl",
        {
            { "http://example.org/P1", "http://example.org/P2" }
        }
    }
};

std::string ParamNameGen(const ::testing::TestParamInfo<Param>& info)
{
    return { info.param.case_name };
}



INSTANTIATE_TEST_SUITE_P(
    ,
    DepsQueries_RetrieveRelatedPersons,
    ::testing::ValuesIn(g_params),
    ParamNameGen);

} // anonymous namespace

} // namespace test
