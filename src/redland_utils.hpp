#if !defined GEN_SANDBOX__REDLAND_UTILS_HPP
#define GEN_SANDBOX__REDLAND_UTILS_HPP

#include <string>

#include <redland.h>
#include <spdlog/spdlog.h>


struct redland_context {
    librdf_world*   world;
    librdf_storage* storage;
    librdf_model*   model;
    librdf_parser*  parser;
    librdf_uri*     base_uri;
};

inline void release_redland_ctx(redland_context* ctx) {
    librdf_free_uri(ctx->base_uri);
    spdlog::debug("Released the Base URI");

    librdf_free_parser(ctx->parser);
    spdlog::debug("Released the Redland Parser");

    librdf_free_model(ctx->model);
    spdlog::debug("Released the Redland Model");

    librdf_free_storage(ctx->storage);
    spdlog::debug("Released the Redland Storage");

    librdf_free_world(ctx->world);
    spdlog::debug("Released the Redland World");

    spdlog::info("Released the Redland Context");

    delete ctx;
}

typedef
    std::unique_ptr<redland_context, decltype(&release_redland_ctx)>
    scoped_redland_ctx;

inline scoped_redland_ctx create_redland_ctx() {
    return { new redland_context(), release_redland_ctx };
}


void load_rdf(librdf_world* world, librdf_model* model, const std::string& input_file_path);

void exec_query(librdf_world* world, librdf_model* model, const std::string& query_text);

#endif // !defined GEN_SANDBOX__REDLAND_UTILS_HPP
