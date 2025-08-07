#if !defined COMMON_FILE_SYSTEM_UTILS_HPP
#define COMMON_FILE_SYSTEM_UTILS_HPP

#include <filesystem>
#include <string>
#include <set>
#include <vector>

#include <fmt/format.h>

namespace common
{

using file_set = std::set<std::filesystem::path>;
using input_files = file_set;

input_files find_input_files(
    const std::filesystem::path& root_dir_path,
    const std::string& extension);

input_files adapt_string_paths(const std::vector<std::string>& paths);

input_files merge_input_files(input_files first, input_files second);

} // namespace common

/** @brief The fmt library formatter for the std::filesystem::path */
template <> struct fmt::formatter<std::filesystem::path>: formatter<string_view>
{
    auto format(const std::filesystem::path& p, format_context& ctx) const
    -> format_context::iterator;
};

#endif // !defined COMMON_FILE_SYSTEM_UTILS_HPP
