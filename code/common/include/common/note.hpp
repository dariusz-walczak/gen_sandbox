#if !defined COMMON_NOTE_HPP
#define COMMON_NOTE_HPP

#include <map>
#include <memory>
#include <string>
#include <variant>

#include <nlohmann/json.hpp>

#include "common/resource.hpp"


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

    /// @brief Variable name
    using VarName = std::string;
    /// @brief Variable value
    using VarValue = std::variant<std::string, int, std::shared_ptr<common::Resource>>;

    Type type;
    /**
     * @brief Note identifier
     *
     * The identifier is used to uniqely identify the note meaning and variable set. Can be used to
     *  identify a static note text or a dynamic note template.
     */
    Id id;
    /**
     * @brief Note variable lookup table
     *
     * When populated the table defines the variables that will be replaced when the dynamic note
     *  template is instantiated.
     */
    std::map<VarName, VarValue> vars;
    /**
     * @brief Diagnostic note template instantiation.
     *
     * The note text instantiated for diagnostic purposes only. It may be used for presentation
     *  purposes but may be difficult to localize or include resource links.
     */
    std::string diagnostic_text;

}; // class Note

nlohmann::json note_to_json(const Note& note);

[[nodiscard]] std::string_view to_string(Note::Type type) noexcept;

inline std::ostream& operator<<(std::ostream& os, Note::Type type)
{
    return os << to_string(type);
}

} // namespace common

#endif // !defined COMMON_NOTE_HPP
