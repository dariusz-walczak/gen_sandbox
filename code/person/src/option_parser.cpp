#include "person/option_parser.hpp"

#include "common/command_line_utils.hpp"
#include "common/spdlog_utils.hpp"


cli_context init_cli_context(spdlog::level::level_enum default_log_level)
{
    cli_context result = {};

    result.parser = std::make_unique<CLI::App>();
    result.parser->name("Person Query Application");
    result.parser->description("<app description>");

    result.parser->add_option(
        "-i,--input", result.options.input_paths,
        "Path to an individual turtle file to be loaded into the RDF model");
    result.parser->add_option(
        "STORAGE", result.options.base_path_raw, "PATH to the turtle files STORAGE")
        ->option_text("PATH")
        ->required()
        ->check(common::validate_existing_dir_path);
    common::add_log_level_cli_option(
        result.parser.get(), result.options.log_level, default_log_level);

    CLI::App* details_cmd = result.parser->add_subcommand(
        "details", "Provide details of a single person");

    details_cmd->add_option(
        "-p,--person", result.options.details_cmd.person_id,
        "Person Local Name in the 'P00000' format")
        ->option_text("PID")
        ->required()
        ->check(common::validate_person_local_name);

    result.parser->add_subcommand("list", "Provide the person list");

    result.parser->add_subcommand("deps", "Provide dependencies list");

    return result;
}
