#include "common/note.hpp"

#include "common/common_exception.hpp"

namespace common
{

namespace
{

bool note_type_valid(Note::Type type) noexcept
{
    return ((type == Note::Type::Info) ||
            (type == Note::Type::Warning) ||
            (type == Note::Type::Error));
}

}

std::string_view to_string(Note::Type type) noexcept
{
    switch (type)
    {
    case Note::Type::Uninitialized: return "uninitialized";
    case Note::Type::Info: return "info";
    case Note::Type::Warning: return "warning";
    case Note::Type::Error: return "error";
    };
    return "invalid";
}

nlohmann::json note_to_json(const Note& note)
{
    if (!note_type_valid(note.type))
    {
        throw common_exception(
            common_exception::error_code::input_contract_error,
            fmt::format(
                "Precondition failure: note.type={} must satisfy note_type_valid",
                to_string(note.type)));
    }

    nlohmann::json result;

    result["type"] = to_string(note.type);
    result["diag"] = note.diagnostic_text;
    result["id"] = note.id;
    result["vars"] = nlohmann::json::object();
    
    for (const auto& [name, value] : note.vars)
    {
        if (std::holds_alternative<std::string>(value))
        {
            result["vars"][name] = std::get<std::string>(value);
        }
        else if (std::holds_alternative<int>(value))
        {
            result["vars"][name] = std::get<int>(value);
        }
        else if (std::holds_alternative<std::shared_ptr<common::Resource>>(value))
        {
            const auto& res = std::get<std::shared_ptr<common::Resource>>(value);
            result["vars"][name] = {
                {"unique_path", res->get_unique_id()},
                {"caption", res->get_caption()}
            };
        }
    }

    return result;
}

} // namespace common
