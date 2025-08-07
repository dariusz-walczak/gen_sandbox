#include "common/spdlog_utils.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>

#include "common/common_exception.hpp"

namespace common
{

// ---[ Internal definitions ]------------------------------------------------------------------ //

namespace internal
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

} // namespace internal

// ---[ Interface implementation ]-------------------------------------------------------------- //

void add_log_level_cli_option(
    CLI::App* app, spdlog::level::level_enum& dest, spdlog::level::level_enum default_log_level)
{
    if (!app)
    {
        spdlog::debug("{}: The `app` argument is null", __func__);

        throw common_exception(
            common_exception::error_code::input_contract_error,
            "The `app` argument is null");
    }

    internal::level_map log_level_map = internal::get_level_map(internal::g_levels);

    app->add_option(
        "--log-level", dest, 
        fmt::format(
            "Set logging level. One of {{{}}}. The default is '{}'.",
            internal::get_level_list_string(internal::g_levels),
            spdlog::level::to_string_view(default_log_level)))
        ->option_text("LEVEL")
        ->default_val(default_log_level)
        ->transform(CLI::CheckedTransformer(log_level_map, CLI::ignore_case));
}

void init_spdlog(spdlog::level::level_enum log_level)
{
    spdlog::set_level(log_level);
    spdlog::set_default_logger(spdlog::stderr_color_mt("stderr_logger"));
}

} // namespace common
