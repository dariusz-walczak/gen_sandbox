#include <iostream>
#include <cstdio>

#include <redland.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "redland_utils.hpp"
#include <CLI/CLI.hpp>
#include <nlohmann/json.hpp>

#include "person.hpp"


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

        SELECT ?person, ?name, ?genderType, ?birthDate, ?deathDate, ?father, ?fatherName, ?motherName
        WHERE {
            ?person a gx:Person ;
                gx:name ?name .
            OPTIONAL {
                ?person gx:gender ?gender .
                ?gender a gx:Gender ;
                    gx:type ?genderType .
            }
            OPTIONAL {
                ?person gx:parent ?father .
                ?father a gx:Person ;
                    gx:name ?fatherName ;
                    gx:gender ?fatherGender .
                ?fatherGender a gx:Gender ;
                    gx:type gx:Male .
            }
            OPTIONAL {
                ?person gx:parent ?mother .
                ?mother a gx:Person ;
                    gx:name ?motherName ;
                    gx:gender ?motherGender .
                ?motherGender a gx:Gender ;
                    gx:type gx:Female .
            }
            ?person gx:birthDate ?birthDate .
            OPTIONAL {
                ?person gx:deathDate ?deathDate
            }

        })"
    };

    nlohmann::json j = {
        {"pi", 3.141},
        {"happy", true},
        {"name", "Niels"},
        {"nothing", nullptr},
        {"answer", {{"everything", 42}}},
        {"list", {1, 0, 2}},
        {"object", {
            {"currency", "USD"},
            {"value", 42.99}
        }}
    };

    std::cout << j.dump(4) << std::endl;

    exec_query_result res = exec_query(redland_ctx->world, redland_ctx->model, query);

    if (!res->success) {
        spdlog::critical("The Redland query execution has failed");

        return 2;
    }

    extract_cb_lut cb_lut = {
        {"genderType", extract_gender_raw}
    };

    print_data_table(extract_data_table(res->results, cb_lut));

    return 0;
}
