#include "person/error.hpp"

#include <sstream>

namespace person
{

person_exception::person_exception()
    : std::exception(), _code(error_code::uninitialized)
{
    set_what(_code, nullptr);
}


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


void person_exception::set_what(error_code code, const char* msg)
{
    std::ostringstream oss;

    switch (code)
    {
    case error_code::uninitialized:
        oss << "<uninitialized>";
        break;
    case error_code::resource_not_found:
        oss << "<resource not found>";
        break;
    case error_code::multiple_resources_found:
        oss << "<multiple resources found>";
        break;
    case error_code::input_contract_error:
        oss << "<input contract error>";
        break;
    case error_code::internal_contract_error:
        oss << "<internal contract error>";
        break;
    default:
        oss << "<invalid code>";
    }

    if (msg && *msg)
    {
        oss << " " << msg;
    }
    _what = oss.str();
}

} // namespace person
