#include <iostream>
#include <cstdio>

#include <redland.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "redland_utils.hpp"
#include <CLI/CLI.hpp>

int main(int argc, char** argv) {
    spdlog::set_level(spdlog::level::info);
    spdlog::set_default_logger(spdlog::stderr_color_mt("stderr_logger"));

    CLI::App app{"My Program Description"};

    std::vector<std::string> input_paths;
    app.add_option(
        "-i,--input", input_paths, "Path to the turtle file to be loaded into the RDF model")
        ->required();

    CLI11_PARSE(app, argc, argv);

    scoped_redland_ctx redland_ctx = create_redland_ctx();

    if (!initialize_redland_ctx(redland_ctx)) {
        spdlog::critical("Redland Context: Failed to initialize");

        return 1;
    }

    for (auto input_path : input_paths) {
        load_rdf(redland_ctx->world, redland_ctx->model, input_path);
    }

    std::cout << std::endl;

    librdf_model_print(redland_ctx->model, stdout);

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

    exec_query(redland_ctx->world, redland_ctx->model, query);

    return 0;
}
