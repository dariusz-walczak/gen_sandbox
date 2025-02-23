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

    const std::string query = R"(
        PREFIX gx: <http://gedcomx.org/>

        SELECT ?person, ?genderType, ?birthDate, ?deathDate, ?father, ?fatherName,
            ?motherName
        WHERE {
            ?person a gx:Person .
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
            FILTER (?person = <)" + person_iri + R"(>)
        })";

    exec_query_result res = exec_query(redland_ctx->world, redland_ctx->model, query);

    if (!res->success) {
        spdlog::critical("The Redland query execution has failed");

        return 2;
    }

    extract_cb_lut cb_lut = {
        {"genderType", extract_gender_raw}
    };

    extract_data_table_result data_tuple = extract_data_table(res->results, cb_lut);
    const data_table& data_table = std::get<1>(data_tuple);

    if (data_table.empty()) {
        spdlog::error("Person {} not found", person_id);
        return 3;
    }

    assert(data_table.size() == 1);

    const data_row& data_row = data_table[0];

    Person person;
    extract_person_gender(person, data_row, "genderType");

    retrieve_result name_res =
        retrieve_person_name(person, person_iri, redland_ctx->world, redland_ctx->model);

    assert(name_res != retrieve_result::Uninitialized);

    if (name_res == retrieve_result::QueryError) {
        spdlog::critical("Name retrieval failed due to a query error");

        return 4;
    }

    if (name_res == retrieve_result::NotFound) {
        spdlog::info("Name of person {} not found", person_id);
    }

    retrieve_result parents_res = retrieve_person_parents(
        person, person_iri, redland_ctx->world, redland_ctx->model);

    assert(parents_res != retrieve_result::Uninitialized);

    if (parents_res == retrieve_result::QueryError) {
        spdlog::critical("Name retrieval failed due to a query error");

        return 4;
    }

    if (parents_res == retrieve_result::NotFound) {
        spdlog::info("Name of person {} not found", person_id);
    }

    nlohmann::json output = person_to_json(person);
    std::cout << output.dump(4) << std::endl;

    return 0;
}
