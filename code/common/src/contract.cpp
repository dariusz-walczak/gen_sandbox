#include "common/contract.hpp"

#include "common/common_exception.hpp"
#include "common/resource.hpp"

#include <spdlog/spdlog.h>


namespace common
{

void ensure_resources_not_null(const std::vector<std::shared_ptr<Resource>>& resources)
{
    const bool null_res_found = std::ranges::any_of(resources, [](const auto& p) { return !p; });
    if (null_res_found)
    {
        spdlog::debug(
            "{}: At least one resource pointer included in the the `resources` sequence is null",
            __func__);

        throw common_exception(
            common_exception::error_code::input_contract_error,
            "Assumption failure: expected non-null input resource pointers; observed at least one"
            " null resource pointer");
    }

    spdlog::debug(
        "{}: All the resource pointers included in the `resources` sequence are not null",
        __func__);
}

} // namespace common
