#include <string>
#include <utility>

#include <gtest/gtest.h>
#include <redland.h>

#include "common/common_exception.hpp"
#include "common/resource.hpp"
#include "test/common/assertions.hpp"

//  Redland related testing utilities
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

namespace test
{
    class tc_error : public std::exception
    {
    public:
        tc_error(const char* msg) : m_what(msg) {}
        [[nodiscard]] const char* what() const noexcept override { return m_what.c_str(); }
    protected:
        std::string m_what;
    };

    struct tc_redland_context
    {
        librdf_world*   world;
        librdf_storage* storage;
        librdf_model*   model;
    };

    void tc_release_redland_ctx(tc_redland_context* ctx)
    {
        librdf_free_model(ctx->model);
        librdf_free_storage(ctx->storage);
        librdf_free_world(ctx->world);
        delete ctx;
    }

    using tc_scoped_redland_ctx =
        std::unique_ptr<tc_redland_context, decltype(&tc_release_redland_ctx)>;

    tc_scoped_redland_ctx tc_initialize_redland_ctx()
    {
        tc_scoped_redland_ctx ctx = { new tc_redland_context(), tc_release_redland_ctx };

        ctx->world = librdf_new_world();

        if (!ctx->world)
        {
            throw tc_error("Test Arrange: Failed to create a Redland world");
        }

        librdf_world_open(ctx->world);

        ctx->storage = librdf_new_storage(ctx->world, "memory", nullptr, nullptr);

        if (!ctx->storage)
        {
            throw tc_error("Test Arrange: Failed to create a Redland storage");
        }

        ctx->model = librdf_new_model(ctx->world, ctx->storage, nullptr);

        if (!ctx->model)
        {
            throw tc_error("Test Arrange: Failed to create a Redland model");
        }

        return ctx;
    }

    librdf_node* tc_create_uri_node(librdf_world* world, const char* uri)
    {
        return librdf_new_node_from_uri_string(
            world, reinterpret_cast<const unsigned char*>(uri));
    }

    void tc_insert_statement(
        librdf_world* world, librdf_model* model,
        librdf_node* subject, librdf_node* predicate, librdf_node* object)
    {
        librdf_statement* statement = librdf_new_statement(world);
        librdf_statement_set_subject(statement, subject);
        librdf_statement_set_predicate(statement, predicate);
        librdf_statement_set_object(statement, object);
        librdf_model_add_statement(model, statement);
        librdf_free_statement(statement);
    }

    void tc_insert_uuu_statement(
        librdf_world* world, librdf_model* model,
        const char* subject, const char* predicate, const char* object)
    {
        librdf_node* subject_node = tc_create_uri_node(world, subject);
        librdf_node* predicate_node = tc_create_uri_node(world, predicate);
        librdf_node* object_node = tc_create_uri_node(world, object);

        tc_insert_statement(world, model, subject_node, predicate_node, object_node);
    }
}

//  The extract_boolean_result function tests
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

// Check if the extract_boolean_result function returns true on a positive ASK query result and
//  false on a negative result.
TEST(RedlandUtils_ExtractBooleanResult, ExpectedAskResult)
{
    test::tc_scoped_redland_ctx ctx = test::tc_initialize_redland_ctx();

    test::tc_insert_uuu_statement(
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
    test::tc_scoped_redland_ctx ctx = test::tc_initialize_redland_ctx();

    test::tc_insert_uuu_statement(
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
    test::tc_scoped_redland_ctx ctx = test::tc_initialize_redland_ctx();

    test::tc_insert_uuu_statement(
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
