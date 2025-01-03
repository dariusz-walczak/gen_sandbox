#include <iostream>
#include <cstdio>

#include <redland.h>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace {
    struct RedlandContext {
        librdf_world*   world;
        librdf_storage* storage;
        librdf_model*   model;
        librdf_parser*  parser;
        librdf_uri*     base_uri;
    };

    RedlandContext ReleaseRedland(const RedlandContext& ctx) {
        librdf_free_uri(ctx.base_uri);
        librdf_free_parser(ctx.parser);
        librdf_free_model(ctx.model);
        librdf_free_storage(ctx.storage);
        librdf_free_world(ctx.world);

        spdlog::debug("Released the Redland context");

        return {};
    }
}

int main() {
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_default_logger(spdlog::stderr_color_mt("stderr_logger"));

    RedlandContext redland_ctx = {};

    redland_ctx.world = librdf_new_world();

    if (!redland_ctx.world) {
        spdlog::critical("Redland Context: Failed to create a Redland World");

        return 1;
    }

    spdlog::debug("Redland Context: Created a Redland World");

    librdf_world_open(redland_ctx.world);

    spdlog::debug("Redland Context: Initialized the Redland World");

    // https://librdf.org/docs/api/redland-storage.html#librdf-new-storage
    redland_ctx.storage = librdf_new_storage(redland_ctx.world, "memory", nullptr, nullptr);

    if (!redland_ctx.storage) {
        spdlog::critical("Redland Context: Failed to create a Redland Storage");

        redland_ctx = ReleaseRedland(redland_ctx);
        return 2;
    }

    spdlog::debug("Redland Context: Created a Redland Storage");

    // https://librdf.org/docs/api/redland-model.html#librdf-new-model
    redland_ctx.model = librdf_new_model(redland_ctx.world, redland_ctx.storage, nullptr);

    if (!redland_ctx.model) {
        spdlog::critical("Redland Context: Failed to create a Redland Model");

        redland_ctx = ReleaseRedland(redland_ctx);
        return 3;
    }

    spdlog::debug("Redland Context: Created a Redland Model");

    // https://librdf.org/docs/api/redland-parser.html#librdf-new-parser
    redland_ctx.parser = librdf_new_parser(redland_ctx.world, "turtle", nullptr, nullptr);

    if (!redland_ctx.parser) {
        spdlog::critical("Redland Context: Failed to create a Redland Parser");

        redland_ctx = ReleaseRedland(redland_ctx);
        return 4;
    }

    spdlog::debug("Redland Context: Created a Redland Parser");

    const unsigned char base_uri_str[] = "https://aurochsoft.com/";
    redland_ctx.base_uri = librdf_new_uri(redland_ctx.world, base_uri_str);

    if (!redland_ctx.base_uri) {
        spdlog::critical("Redland Context: Failed to create the Base URI");

        redland_ctx = ReleaseRedland(redland_ctx);
        return 5;
    }

    spdlog::debug("Redland Context: Created the Base URI");

    const char* input_file_path = "sample.ttl";
    FILE* input_file = fopen(input_file_path, "r");

    if (!input_file) {
        spdlog::error("Failed to open the '{}' file", input_file_path);

        redland_ctx = ReleaseRedland(redland_ctx);
        return 6;
    }

    const int parser_error = librdf_parser_parse_file_handle_into_model(
        redland_ctx.parser, input_file, 0, redland_ctx.base_uri, redland_ctx.model);

    fclose(input_file);

    spdlog::debug("Closed the '{}' file", input_file_path);

    if (parser_error) {
        spdlog::error("Redland Context: Failed to parse the '{}' input file", input_file_path);

        redland_ctx = ReleaseRedland(redland_ctx);
        return 7;
    }

    spdlog::info("Redland Context: Successfully parsed the '{}' input file", input_file_path);

    librdf_model_print(redland_ctx.model, stdout);

    redland_ctx = ReleaseRedland(redland_ctx);

    return 0;
}
