#if !defined TEST_TOOLS_VARIABLE_HPP
#define TEST_TOOLS_VARIABLE_HPP

#include <cstdint>
#include <map>
#include <string>
#include <variant>
#include <vector>

#include "common/note.hpp"

#include "test/tools/resource.hpp"

namespace test::tools
{

struct ComparableVariable
{
    using Value = std::variant<
        int,
        std::string,
        ComparableResource,
        std::vector<ComparableVariable>>;

    std::string name;
    Value value;
};

inline bool operator<(const ComparableVariable& lhs, const ComparableVariable& rhs)
{
    return (lhs.name < rhs.name);
}

inline bool operator==(const ComparableVariable& lhs, const ComparableVariable& rhs)
{
    return ((lhs.name == rhs.name) && (lhs.value == rhs.value));
}

// Comparable Variable Value Utils

[[nodiscard]] ComparableVariable::Value to_comparable(const common::Variable::Value& value);

[[nodiscard]] std::string to_string(
    const ComparableVariable::Value& value, std::uint8_t depth_head=0, std::uint8_t depth_tail=1);

inline void PrintTo(const ComparableVariable::Value& value, std::ostream* os)
{
    *os << to_string(value);
}

// Comparable Variable Utils

[[nodiscard]] ComparableVariable to_comparable(const common::Variable& variable);

[[nodiscard]] std::string to_string(
    const ComparableVariable& variable, std::uint8_t depth_head=0, std::uint8_t depth_tail=1);

inline void PrintTo(const ComparableVariable& variable, std::ostream* os)
{
    *os << to_string(variable);
}

// Comparable Variable Set Utils

using ComparableVariableSet = std::set<ComparableVariable>;

[[nodiscard]] ComparableVariableSet to_comparable(
    const std::set<common::Variable>& variables);

[[nodiscard]] std::string to_string(
    const ComparableVariableSet& vars,
    std::uint8_t depth_head=0, std::uint8_t depth_tail=1);

inline void PrintTo(const ComparableVariableSet& variables, std::ostream* os)
{
    *os << to_string(variables);
}

} // namespace test::tools

#endif // !defined TEST_TOOLS_VARIABLE_HPP
