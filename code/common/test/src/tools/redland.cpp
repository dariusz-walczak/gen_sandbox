#include "test/tools/redland.hpp"

#include "common/redland_utils.hpp"

#include "test/tools/error.hpp"

namespace test::tools
{

void release_redland_ctx(redland_context* ctx)
{
    librdf_free_model(ctx->model);
    librdf_free_storage(ctx->storage);
    librdf_free_world(ctx->world);
    delete ctx;
}

scoped_redland_ctx initialize_redland_ctx()
{
    scoped_redland_ctx ctx = { new redland_context(), release_redland_ctx };

    ctx->world = librdf_new_world();

    if (!ctx->world)
    {
        throw tc_error("Test Arrange: Failed to create a Redland world");
    }

    librdf_world_set_logger(ctx->world, nullptr, common::redland_log_cb);
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

librdf_node* create_uri_node(librdf_world* world, const char* uri)
{
    return librdf_new_node_from_uri_string(
        world, reinterpret_cast<const unsigned char*>(uri));
}

void insert_statement(
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

void insert_uuu_statement(
    librdf_world* world, librdf_model* model,
    const char* subject, const char* predicate, const char* object)
{
    librdf_node* subject_node = create_uri_node(world, subject);
    librdf_node* predicate_node = create_uri_node(world, predicate);
    librdf_node* object_node = create_uri_node(world, object);

    insert_statement(world, model, subject_node, predicate_node, object_node);
}

} // namespace test::tools
