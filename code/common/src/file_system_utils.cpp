#include "file_system_utils.hpp"


std::vector<std::filesystem::path> find_input_files(
    const std::filesystem::path& root_dir_path,
    const std::string& extension)
{
    std::vector<std::filesystem::path> result;

    for (const auto entry : std::filesystem::recursive_directory_iterator(root_dir_path))
    {
        if (entry.exists() && (entry.is_regular_file() || entry.is_symlink()))
        {
            result.push_back(entry.path());
        }
    }

    return result;
}
