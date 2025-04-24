#include "common/common_exception.hpp"

#include <sstream>


common_exception::common_exception(error_code code, const std::string& msg)
    : std::exception(), _code(code)
{
    set_what(code, msg.c_str());
}


common_exception::common_exception(error_code code, const char* msg)
    : std::exception(), _code(code)
{
    set_what(code, msg);
}


void common_exception::set_what(error_code code, const char* msg) {
    std::ostringstream oss;

    switch (code) {
    case error_code::uninitialized:
        oss << "<uninitialized>";
        break;
    case error_code::binding_not_found:
        oss << "<binding not found>";
        break;
    case error_code::data_format_error:
        oss << "<data format error>";
        break;
    default:
        oss << "<invalid code>";
    }

    oss << " " << msg;
    _what = oss.str();
}
