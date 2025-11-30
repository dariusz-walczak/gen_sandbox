#if !defined COMMON_VARIABLE_UTILS_HPP
#define COMMON_VARIABLE_UTILS_HPP

#include "common/contract.hpp"
#include "common/resource.hpp"
#include "common/resource_utils.hpp"
#include "common/variable.hpp"


namespace common
{

/** @brief Construct a sequence Variable object from a sequence of resources
 *
 *  @param[in] var_name the output variable name
 *  @param[in] resources the input resource sequence
 *
 *  @return the constructed variable
 *
 *  @pre All the pointers in the @p resources parameter are not null
 *
 *  @throws common::common_exception (input_contract_error) on the precondition violation
 * */
template<typename Sequence>
requires requires
{
    requires std::ranges::input_range<Sequence>;
    requires is_shared_resource<std::ranges::range_value_t<Sequence>>::value;
}
Variable construct_sequence_variable(
    const std::string& var_name, Sequence&& resources)
{
    ensure_items_not_null(resources);

    Variable result_var = {var_name, {std::vector<Variable>{}}};
    auto& vars = std::get<std::vector<Variable>>(result_var.value);

    if constexpr (requires (Sequence& s) { std::ranges::size(s); })
    {
        vars.reserve(resources.size()); // TEST with set, c-array and a vector
    }

    for (const auto& res : resources)
    {
        vars.emplace_back(Variable{"", res});
    }

    return result_var;
}

} // namespace common

#endif // !defined COMMON_VARIABLE_UTILS_HPP
