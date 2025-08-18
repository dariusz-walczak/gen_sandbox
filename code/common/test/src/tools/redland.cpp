#include "test/tools/redland.hpp"

#include "common/redland_utils.hpp"

#include "test/tools/error.hpp"

namespace test::tools
{

//  Engine Initialization
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

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

//  Data Loading
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

namespace
{

struct load_rdf_ctx
{
    librdf_parser* parser;
    librdf_uri*    base_uri;
};

void release_load_rdf_ctx(load_rdf_ctx* ctx)
{
    librdf_free_uri(ctx->base_uri);
    librdf_free_parser(ctx->parser);
    delete ctx;
}

using scoped_load_rdf_ctx = std::unique_ptr<load_rdf_ctx, decltype(&release_load_rdf_ctx)>;

} // anonymous namespace

void load_rdf(librdf_world* world, librdf_model* model, const std::string& input_file_path)
{
    scoped_load_rdf_ctx ctx = { new load_rdf_ctx(), release_load_rdf_ctx };

    ctx->parser = librdf_new_parser(world, "turtle", nullptr, nullptr);

    if (!ctx->parser)
    {
        throw tc_error("Test Arrange: Failed to create a redland parser");
    }

    ctx->base_uri = librdf_new_uri(
        world, reinterpret_cast<const unsigned char*>("https://aurochsoft.com/"));

    if (!ctx->base_uri)
    {
        throw tc_error("Test Arrange: Failed to create the base uri");
    }

    FILE* input_file = fopen(input_file_path.c_str(), "r");

    if (!input_file)
    {
        throw tc_error(fmt::format("Test Arrange: Failed to open the '{}' file", input_file_path));
    }

    const int parser_error = librdf_parser_parse_file_handle_into_model(
        ctx->parser, input_file, 0, ctx->base_uri, model);

    fclose(input_file);

    if (parser_error)
    {
        throw tc_error(
            fmt::format("Test Arrange: Failed to parse the '{}' input file", input_file_path));
    }
}

//  Data Insertion
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

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
