#include "person/option_parser.hpp"

#include "common/command_line_utils.hpp"
#include "common/spdlog_utils.hpp"

namespace person
{

cli_context init_cli_context(spdlog::level::level_enum default_log_level)
{
    cli_context result = {};

    result.parser = std::make_unique<CLI::App>();
    result.parser->name("Person Query Application");
    result.parser->description("<app description>");

    CLI::Option_group* input_grp =
        result.parser->add_option_group("Input Data", "Input data source paths");

    input_grp->add_option(
        "-i,--input", result.options.input_paths,
        "Path to an individual turtle file to be loaded into the RDF model");
    input_grp->add_option(
        "-s,--src-root-path", result.options.base_path_raw,
        "The source root PATH to be searched for the turtle files to be loaded into the RDF model")
        ->option_text("PATH")
        ->check(common::validate_existing_dir_path);
    input_grp->require_option();

    common::add_log_level_cli_option(
        result.parser.get(), result.options.log_level, default_log_level);

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

    CLI::App* details_cmd = result.parser->add_subcommand(
        "details", "Provide details of a single person");

    details_cmd->add_option(
        "-p,--person", result.options.details_cmd.person_id,
        "Person Local Name in the 'P00000' format")
        ->option_text("PID")
        ->required()
        ->check(common::validate_person_local_name);

    result.parser->add_subcommand("list", "Provide the person list");

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

    {
        CLI::App* targets_cmd = result.parser->add_subcommand(
            "targets", "Provide a list of output targets as a value of the make variable");

        CLI::Option* json_flag = targets_cmd->add_flag(
            "--json", result.options.targets_cmd.json_flag,
            "Generate the JSON targets");
        targets_cmd->add_flag(
            "--html", result.options.targets_cmd.html_flag,
            "Generate the HTML targets")
            ->excludes(json_flag);

        targets_cmd->add_option(
            "--tgt-root", result.options.targets_cmd.tgt_root_path,
            "The PATH that should prefix the resource specific target path (it doesn't have to exist)")
            ->option_text("PATH")
            ->required();
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

    CLI::App* deps_cmd = result.parser->add_subcommand("deps", "Provide dependencies list");

    deps_cmd->add_option(
        "--tgt-root", result.options.deps_cmd.tgt_root_path,
        "The PATH that will prefix the intermediate (json) target paths in the generated"
        " dependencies")
        ->option_text("PATH")
        ->required();

    deps_cmd->add_option(
        "--meta-target", result.options.deps_cmd.meta_target,
        fmt::format(
            "The NAME of the meta target aggregating the generated intermediate file targets."))
        ->required()
        ->option_text("NAME");

    deps_cmd->add_option(
        "--person", result.options.deps_cmd.person_id,
        "The unique IDentifier of the person whose dependencies to print.")
        ->option_text("ID")
        ->required();
    return result;
}

} // namespace person
