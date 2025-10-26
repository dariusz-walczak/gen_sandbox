#if !defined TEST_TOOLS_NOTE_HPP
#define TEST_TOOLS_NOTE_HPP

#include <cstdint>
#include <map>
#include <string>
#include <variant>
#include <vector>

#include "common/note.hpp"

#include "test/tools/resource.hpp"

namespace test::tools
{

using ComparableVarValue = std::variant<std::string, int, ComparableResource>;

[[nodiscard]] ComparableVarValue to_comparable(const common::Note::VarValue& value);

[[nodiscard]] std::string to_string(
    const ComparableVarValue& val, std::uint8_t depth_head=0, std::uint8_t depth_tail=1);

inline void PrintTo(const ComparableVarValue& val, std::ostream* os)
{
    *os << to_string(val);
}

using ComparableVars = std::map<common::Note::VarName, tools::ComparableVarValue>;

[[nodiscard]] ComparableVars to_comparable(const common::Note::Vars& vars);

[[nodiscard]] std::string to_string(
    const ComparableVars& vars, std::uint8_t depth_head=0, std::uint8_t depth_tail=1);

inline void PrintTo(const ComparableVars& vars, std::ostream* os)
{
    *os << to_string(vars);
}

struct ComparableNote
{
    common::Note::Type type;
    common::Note::Id id;
    test::tools::ComparableVars vars;
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
