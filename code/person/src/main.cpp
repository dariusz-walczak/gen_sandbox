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

#include "person/error.hpp"
#include "person/option_parser.hpp"
#include "person/queries.hpp"
#include "person/command/deps.hpp"
#include "person/command/details.hpp"
#include "person/command/list.hpp"


int main(int argc, char** argv) {
    CLI::App parser{};
    app_options options;

    spdlog::set_level(options.log_level);
    spdlog::set_default_logger(spdlog::stderr_color_mt("stderr_logger"));

    init_option_parser(parser, options);
    CLI11_PARSE(parser, argc, argv);

    spdlog::set_level(options.log_level);

    try
    {
        if (parser.got_subcommand("list"))
        {
            run_list_command(options);
        }
        else if (parser.got_subcommand("details"))
        {
            run_details_command(options);
        }
        else if (parser.got_subcommand("deps"))
        {
            run_deps_command(options);
        }
    }
    catch (const person_exception& e)
    {
        spdlog::error("{}: {}", __func__, e.what());

        return 1;
    }

    return 0;
}
