#include "test/tools/note.hpp"

#include <algorithm>
#include <sstream>

#include <gtest/gtest.h>

#include "common/resource.hpp"

#include "test/tools/error.hpp"
#include "test/tools/string.hpp"

namespace test::tools
{

//  Comparable Variable Value
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

ComparableVariable::Value to_comparable(const common::Variable::Value& value)
{
    if (std::holds_alternative<std::string>(value))
    {
        return {std::get<std::string>(value)};
    }
    else if (std::holds_alternative<int>(value))
    {
        return {std::get<int>(value)};
    }
    else if (std::holds_alternative<std::shared_ptr<common::Resource>>(value))
    {
        const auto& resource = std::get<std::shared_ptr<common::Resource>>(value);
        return {ComparableResource{resource->get_uri_str(), resource->get_caption()}};
    }
    else if (std::holds_alternative<std::vector<common::Variable>>(value))
    {
        const auto& src_vars = std::get<std::vector<common::Variable>>(value);
        std::vector<ComparableVariable> dst_vars;
        dst_vars.reserve(src_vars.size());
        std::transform(
            src_vars.cbegin(), src_vars.cend(), std::back_inserter(dst_vars),
            [](const auto& var) { return to_comparable(var); });
        return {dst_vars};
    }
    else
    {
        throw tools::tc_error("Test Assert: Unexpected common::Note::VarValue alternative type");
    }
}

std::string to_string(
    const ComparableVariable::Value& value, std::uint8_t depth_head, std::uint8_t depth_tail)
{
    const std::string indent_head = construct_indent(depth_head);

    std::ostringstream oss;
    oss << indent_head << "ComparableVariable::Value{";

    if (std::holds_alternative<std::string>(value))
    {
        oss << ::testing::PrintToString(std::get<std::string>(value));
    }
    else if (std::holds_alternative<int>(value))
    {
        oss << std::get<int>(value);
    }
    else if (std::holds_alternative<tools::ComparableResource>(value))
    {
        oss << to_string(std::get<tools::ComparableResource>(value));
    }
    else if (std::holds_alternative<std::vector<tools::ComparableVariable>>(value))
    {
        const auto& vars = std::get<std::vector<tools::ComparableVariable>>(value);
        const std::string indent_vector_head = construct_indent(depth_head+1);
        oss << indent_vector_head << "std::vector<ComparableVariable>{";

        if (!vars.empty())
        {
            const std::string indent_vector_item = construct_indent(depth_head+2);
            auto var_it = vars.cbegin();

            if (var_it != vars.cend())
            {
                oss << indent_vector_item << to_string(*var_it);
                ++var_it;
            }

            while (var_it != vars.cend())
            {
                oss << ",\n" << indent_vector_item << to_string(*var_it);
                ++var_it;
            }
        }

        oss << "}";
    }
    else
    {
        throw tools::tc_error("Test Assert: Unexpected ComparableVarValue alternative type");
    }

    oss << "}";
    return oss.str();
}

//  Comparable Variable
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

ComparableVariable to_comparable(const common::Variable& variable)
{
    return ComparableVariable{variable.name, to_comparable(variable.value)};
}

std::string to_string(const ComparableVariable& variable, std::uint8_t depth_head, std::uint8_t depth_tail)
{
    const std::string indent_head = construct_indent(depth_head);
    const std::string indent_tail = construct_indent(depth_tail);
    std::ostringstream oss;
    oss << indent_head << "ComparableVariable{\n"
        << indent_tail << "name=" << ::testing::PrintToString(variable.name) << ",\n"
        << indent_tail << "value=" << to_string(variable.value, 0, depth_tail+1) << "}";
    return oss.str();
}

//  Comparable Variable Set
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

ComparableVariableSet to_comparable(const std::set<common::Variable>& variables)
{
    ComparableVariableSet output;

    for (const auto& var : variables)
    {
        output.insert(to_comparable(var));
    }

    return output;
}

std::string to_string(
    const ComparableVariableSet& vars, std::uint8_t depth_head, std::uint8_t depth_tail)
{
    const std::string indent_head = construct_indent(depth_head);

    std::ostringstream oss;

    oss << indent_head << "ComparableVariableSet{";

    if (!vars.empty())
    {
        const std::string indent_tail = construct_indent(depth_tail);
        auto var_it = vars.cbegin();

        if (var_it != vars.cend())
        {
            oss << indent_tail << to_string(*var_it);
            ++var_it;
        }

        while (var_it != vars.cend())
        {
            oss << ",\n" << indent_tail << to_string(*var_it);
            ++var_it;
        }
    }

    oss << "}";
    return oss.str();
}

} // namespace test::tools
