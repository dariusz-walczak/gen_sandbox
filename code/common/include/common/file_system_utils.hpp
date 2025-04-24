#if !defined COMMON__FILE_SYSTEM_UTILS_HPP
#define COMMON__FILE_SYSTEM_UTILS_HPP

#include <filesystem>
#include <string>
#include <set>
#include <vector>


typedef std::set<std::filesystem::path> input_files;

input_files find_input_files(
    const std::filesystem::path& root_dir_path,
    const std::string& extension);

input_files adapt_string_paths(std::vector<std::string> paths);

input_files merge_input_files(input_files first, input_files second);

#endif // !defined COMMON__FILE_SYSTEM_UTILS_HPP
