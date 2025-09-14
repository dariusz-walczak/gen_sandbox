#include <set>
#include <string>
#include <vector>

#include <gtest/gtest.h>
#include <boost/url.hpp>

#include "test/tools/redland.hpp"

#include "person/queries/common.hpp"


namespace test
{
//  The retrieve_person_uris function tests
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

namespace {

struct Param
{
    const char* case_name;

    struct triple
    {
        const char* subject;
        const char* predicate;
        const char* object;
    };

    std::vector<triple> triples;
    std::set<std::string> iris;
};

class CommonQueries_RetrievePersonIris : public ::testing::TestWithParam<Param> {};

TEST_P(CommonQueries_RetrievePersonIris, NormalSuccessCases)
{
    const Param& param = GetParam();
    test::tools::scoped_redland_ctx ctx = test::tools::initialize_redland_ctx();

    for (const auto& triple : param.triples)
    {
        test::tools::insert_uuu_statement(
            ctx->world, ctx->model,
            triple.subject, triple.predicate, triple.object);
    }

    common::resource_set resources = person::retrieve_person_uris(ctx->world, ctx->model);

    const std::set<boost::urls::url_view> expected_iris{ param.iris.begin(), param.iris.end() };
    std::set<boost::urls::url_view> actual_iris;

    std::transform(
        resources.begin(), resources.end(), std::inserter(actual_iris, actual_iris.end()),
        [](auto const& r) { return r->get_uri(); });

    EXPECT_EQ(expected_iris, actual_iris);
}

const std::vector<Param> g_params {
    {
        "EmptyModel",
        {
        },
        {
        }
    },
    {
        "SinglePersonTriple",
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
    {
        "TwoPersonTriples",
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
    {
        "SingleNonPersonTriple",
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
    {
        "TwoPersonAndSomeNonPersonTriples",
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

std::string ParamNameGen(const ::testing::TestParamInfo<Param>& info)
{
    return { info.param.case_name };
}


INSTANTIATE_TEST_SUITE_P(
    ,
    CommonQueries_RetrievePersonIris,
    ::testing::ValuesIn(g_params));

} // anonymous namespace

} // namespace test
