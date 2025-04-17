#if !defined COMMON__FILE_SYSTEM_UTILS_HPP
#define COMMON__FILE_SYSTEM_UTILS_HPP

#include <filesystem>
#include <string>
#include <vector>


std::vector<std::filesystem::path> find_input_files(
    const std::filesystem::path& root_dir_path,
    const std::string& extension);


#endif // !defined COMMON__FILE_SYSTEM_UTILS_HPP
