#include <string>
#include <utility>

#include <gtest/gtest.h>
#include <redland.h>

#include "common/common_exception.hpp"
#include "common/resource.hpp"
#include "test/tools/assertions.hpp"
#include "test/tools/redland.hpp"


//  The extract_boolean_result function tests
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

// Check if the extract_boolean_result function returns true on a positive ASK query result and
//  false on a negative result.
TEST(RedlandUtils_ExtractBooleanResult, ExpectedAskResult)
{
    test::tools::scoped_redland_ctx ctx = test::tools::initialize_redland_ctx();

    test::tools::insert_uuu_statement(
        ctx->world, ctx->model,
        "http://example.com/P00200",
        "http://gedcomx.org/gender",
        "http://gedcomx.org/Female");

    const std::string positive_query = R"(
        ASK WHERE {
            <http://example.com/P00200> ?predicate ?object
        })";

    const std::string negative_query = R"(
        ASK WHERE {
            <http://example.com/P00404> ?predicate ?object
        })";

    exec_query_result positive_res = exec_query(ctx->world, ctx->model, positive_query);
    exec_query_result negative_res = exec_query(ctx->world, ctx->model, negative_query);

    EXPECT_TRUE(extract_boolean_result(positive_res->results));
    EXPECT_FALSE(extract_boolean_result(negative_res->results));
}

// Check if the extract_boolean_result function has no side effects and can take the same query
//  result twice in a row without the outcome changing.
TEST(RedlandUtils_ExtractBooleanResult, ExpectedPurity)
{
    test::tools::scoped_redland_ctx ctx = test::tools::initialize_redland_ctx();

    test::tools::insert_uuu_statement(
        ctx->world, ctx->model,
        "http://example.com/P00200",
        "http://gedcomx.org/gender",
        "http://gedcomx.org/Female");

    const std::string query = R"(
        ASK WHERE {
            ?subject ?predicate <http://gedcomx.org/Female>
        })";

    exec_query_result query_res = exec_query(ctx->world, ctx->model, query);

    EXPECT_TRUE(extract_boolean_result(query_res->results));
    EXPECT_TRUE(extract_boolean_result(query_res->results));
}

// Check if the extract_boolean_result function throws the common_exception when the results
//  parameter is null
TEST(RedlandUtils_ExtractBooleanResult, NullResultsFailure)
{
    EXPECT_THROW_WITH_CODE(
        extract_boolean_result(nullptr),
        common_exception, common_exception::error_code::input_contract_error);
}

TEST(RedlandUtils_ExtractBooleanResult, ResultsTypeFailure)
{
    test::tools::scoped_redland_ctx ctx = test::tools::initialize_redland_ctx();

    test::tools::insert_uuu_statement(
        ctx->world, ctx->model,
        "http://example.com/P00200",
        "http://gedcomx.org/gender",
        "http://gedcomx.org/Female");

    const std::string positive_query = R"(
        SELECT ?subject
        WHERE
        {
            ?subject ?predicate <http://gedcomx.org/Female>
        })";

    const std::string negative_query = R"(
        SELECT ?subject
        WHERE
        {
            ?subject ?predicate <http://gedcomx.org/Male>
        })";

    exec_query_result positive_res = exec_query(ctx->world, ctx->model, positive_query);
    exec_query_result negative_res = exec_query(ctx->world, ctx->model, negative_query);

    EXPECT_THROW_WITH_CODE(
        extract_boolean_result(positive_res->results),
        common_exception, common_exception::error_code::input_contract_error);
    EXPECT_THROW_WITH_CODE(
        extract_boolean_result(negative_res->results),
        common_exception, common_exception::error_code::input_contract_error);
}
