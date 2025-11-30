#if !defined COMMON_CONTRACT_HPP
#define COMMON_CONTRACT_HPP

#include <algorithm>
#include <memory>
#include <ranges>
#include <vector>

#include <spdlog/spdlog.h>

#include "common/common_exception.hpp"
#include "common/traits.hpp"


namespace common
{

struct Resource;

/** @brief Ensure that the provided sequence of resource pointers doesn't include a null pointer.
 *
 * @param[in] resources The input sequence to be checked
 *
 * @throws common::common_exception (input_contract_error) when at least one of the input sequence
 *     elements is a null pointer */
template<class Sequence>
requires requires
{
    requires std::ranges::input_range<Sequence>;
    requires is_shared_ptr_v<std::ranges::range_value_t<Sequence>>;
}
void ensure_items_not_null(Sequence&& sequence)
{
    const bool null_res_found = std::ranges::any_of(sequence, [](const auto& p) { return !p; });
    if (null_res_found)
    {
        spdlog::debug(
            "{}: At least one pointer included in the the input sequence is null",
            __func__);

        throw common_exception(
            common_exception::error_code::input_contract_error,
            "Assumption failure: expected non-null pointers in the input sequence; observed at"
            " least one null pointer");
    }

    spdlog::debug(
        "{}: All the resource pointers included in the sequence are not null",
        __func__);
}

} // namespace common

#endif // !defined COMMON_CONTRACT_HPP
