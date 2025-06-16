#include "common/file_system_utils.hpp"

#include <spdlog/spdlog.h>


input_files find_input_files(
    const std::filesystem::path& root_dir_path,
    const std::string& extension)
{
    spdlog::trace(
        fmt::format(
            "{}: Recursively searching for all the '{}' files in the '{}' root directory",
            __func__, extension, root_dir_path.string()));

    const std::string ext = {"." + extension};

    input_files result;

    for (const auto entry : std::filesystem::recursive_directory_iterator(root_dir_path))
    {
        if (entry.exists() && entry.is_regular_file())
        {
            if (entry.path().extension().string() == ext) {
                result.insert(std::filesystem::absolute(entry.path()).lexically_normal());
            }
        }
    }

    spdlog::info(
        fmt::format(
            "{}: Found {} {} file{} within the '{}' directory tree",
            __func__, result.size(), extension, (result.size() == 1 ? "" : "s"),
            root_dir_path.string()));

    return result;
}

input_files adapt_string_paths(std::vector<std::string> paths)
{
    input_files result;

    for (const auto path : paths)
    {
        result.insert(std::filesystem::absolute(std::filesystem::path(path)).lexically_normal());
    }

    return result;
}

input_files merge_input_files(input_files first, input_files second)
{
    first.merge(second);
    return first;
}
