#include "common/command_line_utils.hpp"

#include <regex>
#include <sstream>

#include "common/file_system_utils.hpp"

namespace common
{

std::string validate_existing_dir_path(const std::string& raw) {
    auto path = std::filesystem::path(raw);


    bool path_exists = { false };

    try
    {
        path_exists = std::filesystem::exists(path);
    }
    catch (std::filesystem::filesystem_error e)
    {
        return e.what();
    }

    if (!path_exists)
    {
        return fmt::format("the path doesn't exist [{}]", path);
    }

    bool path_is_dir = { false };

    try
    {
        path_is_dir = std::filesystem::is_directory(path);
    }
    catch (std::filesystem::filesystem_error e)
    {
        return e.what();
    }

    if (!path_is_dir)
    {
        return fmt::format("the path isn't a directory [{}]", path);
    }
    return "";
}

} // namespace common
