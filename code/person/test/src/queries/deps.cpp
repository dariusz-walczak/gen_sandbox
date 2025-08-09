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
    {
        {
            {
                "http://example.com/P00001",
                "http://www.w3.org/1999/02/22-rdf-syntax-ns#type",
                "http://gedcomx.org/Person"
            },
            {
                "http://example.com/P00002",
                "http://www.w3.org/1999/02/22-rdf-syntax-ns#type",
                "http://gedcomx.org/Person"
            }
        },
        {
            { "http://example.com/P00001" },
            { "http://example.com/P00002" }
        }
    }
};

INSTANTIATE_TEST_SUITE_P(
    ,
    DepsQueries_RetrievePersonIris,
    ::testing::ValuesIn(g_expected_success_params));

} // namespace test
