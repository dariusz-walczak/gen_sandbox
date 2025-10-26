#include "test/tools/note.hpp"

#include <sstream>

#include <gtest/gtest.h>

#include "common/resource.hpp"

#include "test/tools/error.hpp"
#include "test/tools/string.hpp"

namespace test::tools
{

ComparableVarValue to_comparable(const common::Note::VarValue& value)
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
        return {tools::ComparableResource{resource->get_uri_str(), resource->get_caption()}};
    }
    else
    {
        throw tools::tc_error("Test Assert: Unexpected common::Note::VarValue alternative type");
    }
}

std::string to_string(
    const ComparableVarValue& val, std::uint8_t depth_head, std::uint8_t depth_tail)
{
    const std::string indent_head = construct_indent(depth_head);
    //const std::string indent_tail = construct_indent(depth_head+1);

    std::ostringstream oss;
    oss << indent_head << "ComparableVarValue{";

    if (std::holds_alternative<std::string>(val))
    {
        oss << ::testing::PrintToString(std::get<std::string>(val));
    }
    else if (std::holds_alternative<int>(val))
    {
        oss << std::get<int>(val);
    }
    else if (std::holds_alternative<test::tools::ComparableResource>(val))
    {
        oss << to_string(std::get<test::tools::ComparableResource>(val));
    }
    else
    {
        throw tools::tc_error("Test Assert: Unexpected ComparableVarValue alternative type");
    }

    oss << "}";
    return oss.str();
}

ComparableVars to_comparable(const common::Note::Vars& vars)
{
    ComparableVars output;

    for (const auto& [name, value] : vars)
    {
        output[name] = to_comparable(value);
    }

    return output;
}

std::string to_string(const ComparableVars& vars, std::uint8_t depth_head, std::uint8_t depth_tail)
{
    const std::string indent_head = construct_indent(depth_head);
    std::ostringstream oss;
    oss << indent_head << "ComparableVars{";

    if (vars.empty())
    {
        oss << "}";
        return oss.str();
    }

    const std::string indent_tail = construct_indent(depth_tail);

    oss << "\n";

    const ComparableVars::size_type item_cnt = vars.size();
    auto item_idx = ComparableVars::size_type{0};

    for (const auto& [name, value] : vars)
    {
        oss << indent_tail << ::testing::PrintToString(name) << "=" << to_string(value, 0);

        ++item_idx;

        if (item_idx < item_cnt)
        {
            oss << ",\n";
        }
        else
        {
            oss << "}";
        }
    }

    return oss.str();
}

ComparableNote to_comparable(const common::Note& note)
{
    return tools::ComparableNote{
        note.m_type, note.m_id, to_comparable(note.m_vars), note.m_diagnostic_text
    };
}

std::string to_string(const ComparableNote& note, std::uint8_t depth_head, std::uint8_t depth_tail)
{
    const std::string indent_head = construct_indent(depth_head);
    const std::string indent_tail = construct_indent(depth_tail);
    std::ostringstream oss;

    oss << indent_head << "ComparableNote{\n";
    oss << indent_tail << "type=" << common::to_string(note.type) << ",\n"
        << indent_tail << "id=" << ::testing::PrintToString(note.id) << ",\n"
        << indent_tail << "vars=" << to_string(note.vars, 0, depth_tail+1) << ",\n"
        << indent_tail << "diagnostic_text=" << ::testing::PrintToString(note.diagnostic_text)
        << "}";
    return oss.str();
}

ComparableNotes to_comparable(const std::vector<common::Note>& notes)
{
    std::vector<tools::ComparableNote> output;
    output.reserve(notes.size());

    std::transform(
        notes.cbegin(), notes.cend(), std::back_inserter(output),
        static_cast<ComparableNote(*)(const common::Note& note)>(to_comparable));

    return output;
}

} // namespace test::tools
