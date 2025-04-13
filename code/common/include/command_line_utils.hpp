#if !defined COMMON__COMMAND_LINE_UTILS_HPP
#define COMMON__COMMAND_LINE_UTILS_HPP

#include <filesystem>
#include <string>

#include <fmt/format.h>


std::string validate_person_local_name(const std::string& raw);
std::string validate_existing_dir_path(const std::string& raw);

/** @brief The fmt library formatter for the std::filesystem::path */
template <> struct fmt::formatter<std::filesystem::path>: formatter<string_view>
{
    auto format(std::filesystem::path p, format_context& ctx) const
    -> format_context::iterator;
};

#endif // !defined COMMON__COMMAND_LINE_UTILS_HPP
