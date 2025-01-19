#if !defined GEN_SANDBOX__REDLAND_UTILS_HPP
#define GEN_SANDBOX__REDLAND_UTILS_HPP

#include <string>

#include <redland.h>
#include <spdlog/spdlog.h>


struct redland_context {
    librdf_world*   world;
    librdf_storage* storage;
    librdf_model*   model;
};

void release_redland_ctx(redland_context* ctx);

typedef
    std::unique_ptr<redland_context, decltype(&release_redland_ctx)>
    scoped_redland_ctx;

scoped_redland_ctx create_redland_ctx();


void load_rdf(librdf_world* world, librdf_model* model, const std::string& input_file_path);

void exec_query(librdf_world* world, librdf_model* model, const std::string& query_text);

#endif // !defined GEN_SANDBOX__REDLAND_UTILS_HPP
