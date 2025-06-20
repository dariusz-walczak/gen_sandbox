#if !defined COMMON_FILE_SYSTEM_UTILS_HPP
#define COMMON_FILE_SYSTEM_UTILS_HPP

#include <filesystem>
#include <string>
#include <set>
#include <vector>


using input_files = std::set<std::filesystem::path>;

input_files find_input_files(
    const std::filesystem::path& root_dir_path,
    const std::string& extension);

input_files adapt_string_paths(const std::vector<std::string>& paths);

input_files merge_input_files(input_files first, input_files second);

#endif // !defined COMMON_FILE_SYSTEM_UTILS_HPP
