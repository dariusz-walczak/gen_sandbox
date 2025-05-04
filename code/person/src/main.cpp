#include <filesystem>
#include <iostream>
#include <cstdio>

#include <redland.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <nlohmann/json.hpp>

#include "common/command_line_utils.hpp"
#include "common/file_system_utils.hpp"
#include "common/person.hpp"
#include "common/redland_utils.hpp"

#include "person/option_parser.hpp"
#include "person/queries.hpp"


int main(int argc, char** argv) {
    spdlog::set_level(spdlog::level::trace);
    spdlog::set_default_logger(spdlog::stderr_color_mt("stderr_logger"));

    CLI::App parser{};
    app_options options;

    init_option_parser(parser, options);

    CLI11_PARSE(parser, argc, argv);

    auto base_path = std::filesystem::path(options.base_path_raw);

    auto all_input_paths = merge_input_files(
        find_input_files(base_path, ".ttl"),
        adapt_string_paths(options.input_paths));

    scoped_redland_ctx redland_ctx = create_redland_ctx();

    if (!initialize_redland_ctx(redland_ctx)) {
        spdlog::critical("Redland Context: Failed to initialize");

        return 1;
    }

    for (auto input_path : all_input_paths) {
        load_rdf(redland_ctx->world, redland_ctx->model, input_path.string());
    }

#if 0
    std::cout << std::endl;

    librdf_model_print(redland_ctx->model, stdout);

    std::cout << std::endl;
#endif

    if (parser.got_subcommand("list"))
    {
        spdlog::error("The list command is not implemented");
    }
    else if (parser.got_subcommand("details"))
    {
        const std::string person_iri = compose_person_iri(options.details_cmd.person_id);

        Person person;

        retrieve_result person_res =
            retrieve_person_base_data_opt(person, person_iri, redland_ctx->world, redland_ctx->model);

        if (person_res == retrieve_result::NotFound) {
            spdlog::info("Person {} not found", options.details_cmd.person_id);

            return 2;
        }

        retrieve_person_name(person, person_iri, redland_ctx->world, redland_ctx->model);
        retrieve_person_parents(person, person_iri, redland_ctx->world, redland_ctx->model);
        retrieve_person_partners(person, person_iri, redland_ctx->world, redland_ctx->model);
        retrieve_person_children(person, person_iri, redland_ctx->world, redland_ctx->model);

        nlohmann::json output = person_to_json(person);
        std::cout << output.dump(4) << std::endl;
    }

    return 0;
}
