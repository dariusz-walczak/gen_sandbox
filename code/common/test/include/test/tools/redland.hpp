#if !defined TEST_TOOLS_REDLAND_HPP
#define TEST_TOOLS_REDLAND_HPP

#include <memory>

#include <redland.h>

namespace test::tools
{

struct redland_context
{
    librdf_world*   world;
    librdf_storage* storage;
    librdf_model*   model;
};

void release_redland_ctx(redland_context* ctx);

using scoped_redland_ctx = std::unique_ptr<redland_context, decltype(&release_redland_ctx)>;

scoped_redland_ctx initialize_redland_ctx();

librdf_node* create_uri_node(librdf_world* world, const char* uri);
void insert_statement(
    librdf_world* world, librdf_model* model,
    librdf_node* subject, librdf_node* predicate, librdf_node* object);

void insert_uuu_statement(
    librdf_world* world, librdf_model* model,
    const char* subject, const char* predicate, const char* object);

} // namespace test::tools

#endif // !defined TEST_TOOLS_REDLAND_HPP
