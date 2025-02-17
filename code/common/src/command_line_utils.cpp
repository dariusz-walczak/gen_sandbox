#include "command_line_utils.hpp"

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
