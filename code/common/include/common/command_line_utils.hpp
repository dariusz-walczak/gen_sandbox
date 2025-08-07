#if !defined COMMON_COMMAND_LINE_UTILS_HPP
#define COMMON_COMMAND_LINE_UTILS_HPP

#include <filesystem>
#include <string>

namespace common
{

std::string validate_person_local_name(const std::string& raw);
std::string validate_existing_dir_path(const std::string& raw);

} // namespace common

#endif // !defined COMMON_COMMAND_LINE_UTILS_HPP
