#include "common/data_table.hpp"

#include <spdlog/spdlog.h>

#include "common/common_exception.hpp"


namespace common
{

std::vector<std::string> extract_resource_uri_seq(
    const common::data_table& data_table, const std::string& binding_name)
{
    spdlog::trace("{}: Entry checkpoint", __func__);

    std::vector<std::string> result_seq;
    result_seq.reserve(data_table.size());

    for (const common::data_row& row : data_table)
    {
        auto uri_it = common::get_binding_value_req(row, binding_name);
        result_seq.emplace_back(uri_it->second);
    }

    return result_seq;
}

bool get_boolean_value_req(const data_row& row, const std::string& binding_name)
{
    spdlog::trace("{}: Entry checkpoint (binding_name='{}')", __func__, binding_name);

    // Exceptional path (binding not found): Propagate the common_exception
    const auto& value_it = get_binding_value_req(row, binding_name);

    // Normal path (binding found): Continue the execution
    const auto& value_raw = value_it->second;

    if (value_raw == "true")
    {
        return true;
    }
    else if (value_raw == "false")
    {
        return false;
    }
    else
    {
        spdlog::warn(
            "{}: The value of the '{}' binding is not a valid boolean: {}",
            __func__, binding_name, value_raw);

        throw common_exception(
            common_exception::error_code::input_contract_error,
            fmt::format("The value of the '{}' binding is not a valid boolean", binding_name));
    }
}

} // namespace common
