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

    CLI::App* deps_cmd = result.parser->add_subcommand("deps", "Provide dependencies list");

    deps_cmd->add_option(
        "--src-root-symbol", result.options.deps_cmd.src_root_symbol,
        "The SYMBOL that should replace the source root path in the generated dependencies")
        ->option_text("SYMBOL")
        ->required();

    deps_cmd->add_option(
        "--int-root-symbol", result.options.deps_cmd.int_root_symbol,
        "The SYMBOL that should replace the intermediate root path in the generated dependencies")
        ->option_text("SYMBOL")
        ->required();

    deps_cmd->add_option(
        "--out-root-symbol", result.options.deps_cmd.out_root_symbol,
        "The SYMBOL that should replace the output root path in the generated dependencies")
        ->option_text("SYMBOL")
        ->required();

    const char* int_meta_target_default = "int_person_all";

    deps_cmd->add_option(
        "--int-meta-target", result.options.deps_cmd.int_meta_target,
        fmt::format(
            "The NAME of the meta target aggregating the generated intermediate file targets."
            " The default is '{}'.", int_meta_target_default))
        ->option_text("NAME")
        ->default_val(int_meta_target_default);

    const char* out_meta_target_default = "out_person_all";

    deps_cmd->add_option(
        "--out-meta-target", result.options.deps_cmd.out_meta_target,
        fmt::format(
            "The NAME of the meta target aggregating the generated output file targets."
            " The default is '{}'.", out_meta_target_default))
        ->option_text("NAME")
        ->default_val(out_meta_target_default);

    return result;
}

} // namespace person
