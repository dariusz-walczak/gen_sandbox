#include "common/common_exception.hpp"

#include <string>
#include <sstream>

namespace common
{

namespace
{

constexpr std::string_view get_error_code_name(common_exception::error_code code)
{
    using error_code = common_exception::error_code;

    switch (code) {
    case error_code::uninitialized: return "uninitialized";
    case error_code::general_runtime_error: return "general runtime error";
    case error_code::redland_initialization_failed: return "redland initialization failed";
    case error_code::redland_unexpected_behavior: return "redland unexpected behavior";
    case error_code::binding_not_found: return "binding not found";
    case error_code::data_format_error: return "data format error";
    case error_code::data_size_error: return "data size error";
    case error_code::input_contract_error: return "input contract error";
    default: return "invalid code";
    }
}

} // anonymous namespace

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
    oss << msg << " [" << get_error_code_name(code) << ":" << int(code) << "]";
    _what = oss.str();
}

} // namespace common
