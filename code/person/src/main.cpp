#include <iostream>
#include <cstdio>

#include <redland.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "redland_utils.hpp"
#include <CLI/CLI.hpp>
#include <nlohmann/json.hpp>

#include "person.hpp"
#include "command_line_utils.hpp"
#include "queries.hpp"


int main(int argc, char** argv) {
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_default_logger(spdlog::stderr_color_mt("stderr_logger"));

    CLI::App app{"My Program Description"};

    std::vector<std::string> input_paths;
    std::string person_id;
    app.add_option(
        "-i,--input", input_paths, "Path to the turtle file to be loaded into the RDF model")
        ->required();
    app.add_option(
        "-p,--person", person_id, "Person Local Name in the 'P00000' format")
        ->option_text("PID")
        ->required()
        ->check(validate_person_local_name);

    CLI11_PARSE(app, argc, argv);

    scoped_redland_ctx redland_ctx = create_redland_ctx();

    if (!initialize_redland_ctx(redland_ctx)) {
        spdlog::critical("Redland Context: Failed to initialize");

        return 1;
    }

    for (auto input_path : input_paths) {
        load_rdf(redland_ctx->world, redland_ctx->model, input_path);
    }

#if 0
    std::cout << std::endl;

    librdf_model_print(redland_ctx->model, stdout);

    std::cout << std::endl;
#endif

    const std::string person_iri = "http://example.org/" + person_id;

    Person person;

    retrieve_result person_res =
        retrieve_person(person, person_iri, redland_ctx->world, redland_ctx->model);

    assert(person_res != retrieve_result::Uninitialized);

    if (person_res == retrieve_result::NotFound) {
        spdlog::info("Person {} not found", person_id);

        return 3;
    }

    retrieve_result name_res =
        retrieve_person_name(person, person_iri, redland_ctx->world, redland_ctx->model);

    assert(name_res != retrieve_result::Uninitialized);

    if (name_res == retrieve_result::NotFound) {
        spdlog::info("Name of person {} not found", person_id);
    }

    retrieve_result parents_res = retrieve_person_parents(
        person, person_iri, redland_ctx->world, redland_ctx->model);

    assert(parents_res != retrieve_result::Uninitialized);

    if (parents_res == retrieve_result::NotFound) {
        spdlog::info("Name of person {} not found", person_id);
    }

    nlohmann::json output = person_to_json(person);
    std::cout << output.dump(4) << std::endl;

    return 0;
}
