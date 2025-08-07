#if !defined COMMON_SPDLOG_UTILS_HPP
#define COMMON_SPDLOG_UTILS_HPP

#include <CLI/CLI.hpp>
#include <spdlog/spdlog.h>

namespace common
{

void add_log_level_cli_option(
    CLI::App* app,
    spdlog::level::level_enum& dest,
    spdlog::level::level_enum default_log_level);

void init_spdlog(spdlog::level::level_enum log_level);

} // namespace common

#endif // !defined COMMON_SPDLOG_UTILS_HPP
