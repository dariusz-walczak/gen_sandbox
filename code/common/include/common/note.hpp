#if !defined COMMON_NOTE_HPP
#define COMMON_NOTE_HPP

#include <map>
#include <memory>
#include <set>
#include <string>
#include <variant>

#include <nlohmann/json.hpp>

#include "common/variable.hpp"

namespace common
{

class Note
{
public:
    enum class Type : std::uint8_t
    {
        Uninitialized = 0,
        Info,
        Warning,
        Error
    };

    /// @brief Unique note identifier
    using Id = std::string;

    Type m_type { Type::Uninitialized };
    /**
     * @brief Note identifier
     *
     * The identifier is used to uniqely identify the note meaning and variable set. Can be used to
     *  identify a static note text or a dynamic note template.
     */
    Id m_id;
    /**
     * @brief Note variable lookup table
     *
     * Set of variables to be replaced during the dynamic note template is instantiation.
     */
    std::set<Variable> m_vars;
    /**
     * @brief Diagnostic note template instantiation.
     *
     * The note text instantiated for diagnostic purposes only. It may be used for presentation
     *  purposes but may be difficult to localize or include resource links.
     */
    std::string m_diagnostic_text;

    Note(
        Type type, Id id, std::set<Variable> vars, std::string diagnostic_text)
        : m_type(type), m_id(std::move(id)), m_vars(std::move(vars)),
          m_diagnostic_text(std::move(diagnostic_text)) {}

}; // class Note

nlohmann::json note_to_json(const Note& note);

[[nodiscard]] std::string_view to_string(Note::Type type) noexcept;

inline std::ostream& operator<<(std::ostream& os, Note::Type type)
{
    return os << to_string(type);
}

} // namespace common

#endif // !defined COMMON_NOTE_HPP
