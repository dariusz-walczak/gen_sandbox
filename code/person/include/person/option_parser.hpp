#if !defined PERSON_OPTION_PARSER_HPP
#define PERSON_OPTION_PARSER_HPP

#include <memory>
#include <string>
#include <vector>

#include <CLI/CLI.hpp>
#include <spdlog/spdlog.h>


struct cli_options
{
    std::vector<std::string> input_paths;
    std::string base_path_raw;
    spdlog::level::level_enum log_level;

    struct details
    {
        std::string person_id;
    } details_cmd;

    struct deps
    {
        std::string src_root_symbol;
        std::string int_root_symbol;
        std::string out_root_symbol;
        std::string int_meta_target;
        std::string out_meta_target;
    } deps_cmd;
};

struct cli_context
{
    /** The CLI::App class is wrapped in a std::unique_ptr to allow cli_context objects
     *   to be returned by value.
     *
     *  The CLI::App class has its copy constructor explicitly deleted. As a result:
     *  * Its move constructor is not implicitly generated.
     *  * The cli_context struct’s copy and move constructors are also not generated.
     *  * Returning a cli_context object by value becomes impossible.
     *
     *  Since std::unique_ptr is moveable (but not copyable), wrapping CLI::App in it
     *   makes cli_context move-constructible and thus returnable by value. */
    std::unique_ptr<CLI::App> parser;
    cli_options options;
};

cli_context init_cli_context(spdlog::level::level_enum default_log_level);

#endif // !defined PERSON_OPTION_PARSER_HPP
