#include "person/option_parser.hpp"

#include "common/command_line_utils.hpp"


namespace
{
    std::vector<spdlog::level::level_enum> g_levels = {
        spdlog::level::trace,
        spdlog::level::debug,
        spdlog::level::info,
        spdlog::level::warn,
        spdlog::level::err,
        spdlog::level::critical,
        spdlog::level::off
    };

    using level_map = std::map<std::string, spdlog::level::level_enum>;
}

namespace
{
/** Convert spdlog::string_view_t to std::string_view
 *
 * This is a convenience function needed for two reasons:
 * * There is no std::ostringstream::operator<< that takes spdlog::string_view_t
 * * No direct conversion from spdlog::string_view_t to std::string_view is possible */
std::string_view to_std_string_view(const spdlog::string_view_t& sv)
{
    return {sv.data(), sv.size()};
}

/** Convert spdlog::string_view_t to std::string
 *
 * This is a convenience function needed because no direct conversion from spdlog::string_view_t to
 *  std::string is possible */
std::string to_std_string(const spdlog::string_view_t& sv)
{
    return {sv.data(), sv.size()};
}
} // namespace

std::string get_level_list_string(const std::vector<spdlog::level::level_enum>& levels)
{
    std::ostringstream oss;

    for (size_t i=0; i<levels.size(); ++i)
    {
        oss << "{" << to_std_string_view(spdlog::level::to_string_view(levels[i])) << "|" <<
            levels[i] << "}";

        if (i+1 < levels.size())
        {
            oss << ", ";
        }
    }

    return oss.str();
}

level_map get_level_map(const std::vector<spdlog::level::level_enum>& levels)
{
    level_map result_map;

    for (auto level : levels)
    {
        std::string level_str = to_std_string(spdlog::level::to_string_view(level));
        result_map[level_str] = level;
    }

    return result_map;
}


void init_option_parser(CLI::App& parser, app_options& result)
{
    parser.name("Person Query Application");
    parser.description("<app description>");

    level_map log_level_map = get_level_map(g_levels);

    parser.add_option(
        "-i,--input", result.input_paths,
        "Path to an individual turtle file to be loaded into the RDF model");
    parser.add_option(
        "STORAGE", result.base_path_raw, "PATH to the turtle files STORAGE")
        ->option_text("PATH")
        ->required()
        ->check(validate_existing_dir_path);
    const auto default_log_level = spdlog::level::info;
    parser.add_option(
        "--log-level", result.log_level,
        fmt::format(
            "Set logging level. One of {{{}}}. The default is '{}'.",
            get_level_list_string(g_levels),
            spdlog::level::to_string_view(default_log_level)))
        ->option_text("LEVEL")
        ->default_val(default_log_level)
        ->transform(CLI::CheckedTransformer(log_level_map, CLI::ignore_case))
        ->default_val("info");

    CLI::App* details_cmd = parser.add_subcommand("details", "Provide details of a single person");

    details_cmd->add_option(
        "-p,--person", result.details_cmd.person_id,
        "Person Local Name in the 'P00000' format")
        ->option_text("PID")
        ->required()
        ->check(validate_person_local_name);

    parser.add_subcommand("list", "Provide the person list");

    parser.add_subcommand("deps", "Provide dependencies list");
}
