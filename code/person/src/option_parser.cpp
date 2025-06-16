#include "person/option_parser.hpp"

#include "common/command_line_utils.hpp"


void init_option_parser(CLI::App& parser, app_options& result)
{
    parser.name("Person Query Application");
    parser.description("<app description>");

    parser.add_option(
        "-i,--input", result.input_paths,
        "Path to an individual turtle file to be loaded into the RDF model");
    parser.add_option(
        "STORAGE", result.base_path_raw, "PATH to the turtle files STORAGE")
        ->option_text("PATH")
        ->required()
        ->check(validate_existing_dir_path);

    CLI::App* details_cmd = parser.add_subcommand("details", "Provide details of a single person");

    details_cmd->add_option(
        "-p,--person", result.details_cmd.person_id,
        "Person Local Name in the 'P00000' format")
        ->option_text("PID")
        ->required()
        ->check(validate_person_local_name);

    CLI::App* list_cmd = parser.add_subcommand("list", "Provide the person list");
}
