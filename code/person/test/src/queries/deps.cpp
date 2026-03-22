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
        "data/queries/deps/retrieve_related_persons/model-00.ttl",
        {
            { "http://example.org/P1", "http://example.org/P2" }
        }
    },
    {
        "SingleFamilyPlusOneUnrelated",
        "data/queries/deps/retrieve_related_persons/model-01.ttl",
        {
            { "http://example.org/P1", "http://example.org/P2" },
            { "http://example.org/P1", "http://example.org/P3" },
            { "http://example.org/P2", "http://example.org/P3" }
        }
    },
    {
        "DuplicateRelation",
        "data/queries/deps/retrieve_related_persons/model-02.ttl",
        {
            { "http://example.org/P1", "http://example.org/P2" }
        }
    },
    {
        "SingleFamilyInferred",
        "data/queries/deps/retrieve_related_persons/model-03_single-family-inferred.ttl",
        {
            { "http://example.org/P1", "http://example.org/P2" },
            { "http://example.org/P1", "http://example.org/P3" },
            { "http://example.org/P2", "http://example.org/P3" }
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
