#include "common/variable.hpp"

#include <fmt/format.h>

#include "common/common_exception.hpp"
#include "common/resource.hpp"

namespace common
{

nlohmann::json variable_to_json_int(const Variable& var, int current_depth)
{
    if (current_depth <= 0)
    {
        throw common_exception(
            common_exception::error_code::input_contract_error,
            fmt::format(
                "Precondition failure: expected maximum variable depth <= {}",
                k_variable_max_depth));
    }

    if (std::holds_alternative<std::string>(var.value))
    {
        return {
            {"name", var.name},
            {"type", "string"},
            {"value", std::get<std::string>(var.value)}
        };
    }
    else if (std::holds_alternative<int>(var.value))
    {
        return {
            {"name", var.name},
            {"type", "integer"},
            {"value", std::get<int>(var.value)}
        };
    }
    else if (std::holds_alternative<std::shared_ptr<Resource>>(var.value))
    {
        const auto& res = std::get<std::shared_ptr<Resource>>(var.value);
        return {
            {"name", var.name},
            {"type", "resource"},
            {
                "value", {
                    {"unique_path", res->get_unique_id()},
                    {"caption", res->get_caption()}
                }
            }
        };
    }
    else if (std::holds_alternative<std::vector<Variable>>(var.value))
    {
        nlohmann::json result = {
            {"name", var.name},
            {"type", "sequence"},
            {"value", nlohmann::json::array()}
        };

        for (const auto& inner_var : std::get<std::vector<Variable>>(var.value))
        {
            result["value"].push_back(variable_to_json_int(inner_var, current_depth-1));
        }

        return result;
    }
    else
    {
        throw common_exception(
            common_exception::error_code::internal_contract_error,
            "Unexpected Note::VarValue alternative type");
    }
}

nlohmann::json variable_to_json(const Variable& var)
{
    return variable_to_json_int(var, k_variable_max_depth);
}

} // namespace common
