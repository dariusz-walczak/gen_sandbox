#include <iostream>
#include <cstdio>

#include <redland.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "redland_utils.hpp"


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
    spdlog::set_level(spdlog::level::info);
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

    load_rdf(redland_ctx.world, redland_ctx.model, "sample.ttl");

    std::cout << std::endl;

    librdf_model_print(redland_ctx.model, stdout);

    std::cout << std::endl;

    const std::string query = {
        R"(
        PREFIX gx: <http://gedcomx.org/>

        SELECT ?person, ?name, ?gender, ?birthDate, ?deathDate
        WHERE {
            ?person a gx:Person ;
                gx:name ?name .
            OPTIONAL {
                ?person gx:gender ?gender
            }
            ?person gx:birthDate ?birthDate .
            OPTIONAL {
                ?person gx:deathDate ?deathDate
            }

        })"
    };

    exec_query(redland_ctx.world, redland_ctx.model, query);

    redland_ctx = ReleaseRedland(redland_ctx);

    return 0;
}
