#include "person/error.hpp"

#include <sstream>


person_exception::person_exception(error_code code, const std::string& msg)
    : std::exception(), _code(code)
{
    set_what(code, msg.c_str());
}


person_exception::person_exception(error_code code, const char* msg)
    : std::exception(), _code(code)
{
    set_what(code, msg);
}


void person_exception::set_what(error_code code, const char* msg) {
    std::ostringstream oss;

    switch (code) {
    case error_code::uninitialized:
        oss << "<uninitialized>";
        break;
    case error_code::query_error:
        oss << "<query error>";
        break;
    case error_code::resource_not_found:
        oss << "<not found>";
        break;
    default:
        oss << "<invalid code>";
    }

    oss << " " << msg;
    _what = oss.str();
}
