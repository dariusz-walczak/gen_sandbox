#if !defined TEST_TOOLS_NOTE_HPP
#define TEST_TOOLS_NOTE_HPP

#include <cstdint>
#include <map>
#include <string>
#include <variant>
#include <vector>

#include "common/note.hpp"

#include "test/tools/resource.hpp"
#include "test/tools/variable.hpp"

namespace test::tools
{

struct ComparableNote
{
    common::Note::Type type;
    common::Note::Id id;
    std::set<ComparableVariable> vars;
    std::string diagnostic_text;

    bool operator==(const ComparableNote& other) const {
        return ((type == other.type) &&
                (id == other.id) &&
                (vars == other.vars) &&
                (diagnostic_text == other.diagnostic_text));
    }
};

[[nodiscard]] ComparableNote to_comparable(const common::Note& note);
[[nodiscard]] std::string to_string(const ComparableNote& note, std::uint8_t depth_head=0, std::uint8_t depth_tail=1);

inline void PrintTo(const ComparableNote& note, std::ostream* os)
{
    *os << to_string(note, 0, 3);
}

using ComparableNotes = std::vector<ComparableNote>;

[[nodiscard]] ComparableNotes to_comparable(const std::vector<common::Note>& notes);

} // namespace test::tools

#endif // !defined TEST_TOOLS_NOTE_HPP
