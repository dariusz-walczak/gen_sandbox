#include "common/command_line_utils.hpp"

#include <regex>
#include <sstream>


std::string validate_person_local_name(const std::string& raw) {
    if (!std::regex_match(raw, std::regex(R"(^P\d{5}$)"))) {
        std::ostringstream oss;
        oss << "The '" << raw << "' value doesn't match the 'P00000' person id format";
        return oss.str();
    } else {
        return "";
    }
}


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


auto fmt::formatter<std::filesystem::path>::format(
    const std::filesystem::path& p, format_context& ctx) const
    -> format_context::iterator
{
    return formatter<string_view>::format(p.string(), ctx);
}
