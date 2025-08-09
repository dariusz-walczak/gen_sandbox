#include <set>
#include <string>
#include <vector>

#include <gtest/gtest.h>
#include <boost/url.hpp>

#include "test/tools/redland.hpp"

#include "person/queries/deps.hpp"

namespace test
{

//  The retrieve_person_iris function tests
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

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

    common::resource_set resources = retrieve_person_iris(ctx->world, ctx->model);

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

} // namespace test
