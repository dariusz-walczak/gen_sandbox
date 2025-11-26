#if !defined COMMON_VARIABLE_HPP
#define COMMON_VARIABLE_HPP

#include <variant>
#include <memory>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace common
{

/**
 * @brief Maximum depth of the common::Variable variable tree which is supported by the
 *     @ref variable_to_json function.
 */
inline constexpr int k_variable_max_depth = 30;

struct Resource;
struct Variable;

struct Variable {
    using Value = std::variant<
        int,
        std::string,
        std::shared_ptr<Resource>,
        std::vector<Variable>
    >;

    /**
     * @brief Variable name.
     *
     * Used as the placeholder identifier in a note template. Every placeholder with this name is
     *  replaced with the rendered value of this variable.
     */
    std::string name;

    /**
     * @brief Variable value.
     *
     * Holds the data used when rendering this variable into a note template.
     */
    Value value;
};

inline bool operator<(const Variable& lhs, const Variable& rhs)
{
    return (lhs.name < rhs.name);
}

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
Variable construct_sequence_variable(
    const std::string& var_name, const std::vector<std::shared_ptr<Resource>>& resources);

nlohmann::json variable_to_json(const Variable& var);

} // namespace common

#endif // !defined COMMON_VARIABLE_HPP
