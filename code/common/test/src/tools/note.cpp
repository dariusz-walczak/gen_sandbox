#include "test/tools/note.hpp"

#include <sstream>

#include <gtest/gtest.h>

#include "common/resource.hpp"

#include "test/tools/error.hpp"
#include "test/tools/string.hpp"

namespace test::tools
{

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
