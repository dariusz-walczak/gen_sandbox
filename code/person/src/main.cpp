#include <filesystem>
#include <iostream>
#include <cstdio>

#include <redland.h>
#include <spdlog/spdlog.h>

#include <nlohmann/json.hpp>

#include "common/command_line_utils.hpp"
#include "common/file_system_utils.hpp"
#include "common/person.hpp"
#include "common/redland_utils.hpp"
#include "common/spdlog_utils.hpp"

#include "person/error.hpp"
#include "person/option_parser.hpp"
#include "person/queries/common.hpp"
#include "person/command/deps.hpp"
#include "person/command/details.hpp"
#include "person/command/list.hpp"


int run_main(int argc, char** argv) {
    spdlog::level::level_enum default_log_level = spdlog::level::info;
    init_spdlog(default_log_level);

    cli_context cli_ctx = init_cli_context(default_log_level);
    CLI11_PARSE(*cli_ctx.parser, argc, argv);

    /* Override the initial log level with the level specified through command line arguments (or
     * taken from the default). */
    spdlog::set_level(cli_ctx.options.log_level);

    if (cli_ctx.parser->got_subcommand("list"))
    {
        run_list_command(cli_ctx.options);
    }
    else if (cli_ctx.parser->got_subcommand("details"))
    {
        run_details_command(cli_ctx.options);
    }
    else if (cli_ctx.parser->got_subcommand("deps"))
    {
        run_deps_command(cli_ctx.options);
    }

    return 0;
}

int main(int argc, char** argv)
{
    try
    {
        return run_main(argc, argv);
    }
    catch (const person_exception& e)
    {
        std::cerr << "ERROR: " << e.what() << "\n";

        return 1;
    }
    catch (const std::exception& e)
    {
        std::cerr << "ERROR: An unhandled exception occurred: " << e.what() << "\n";
    }
    catch (...)
    {
        std::cerr << "ERROR: An unhandled, unrecognized exception occurred\n";
    }
}
