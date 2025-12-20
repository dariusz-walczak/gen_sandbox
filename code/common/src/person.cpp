#include "common/person.hpp"

#include <cassert>
#include <chrono>
#include <format>
#include <sstream>

#include <spdlog/spdlog.h>

#include "common/common_exception.hpp"

namespace common
{

namespace
{
const char* g_male = "male";
const char* g_female = "female";

common::Note create_invalid_gender_note()
{
    return {
        common::Note::Type::Error, std::string(k_invalid_gender_note_id),
        {},
        "Invalid gender value"
    };
}

common::Note create_unspecified_gender_note()
{
    return {
        common::Note::Type::Info, std::string(k_unspecified_gender_note_id),
        {},
        "Unspecified gender"
    };
}

} // anonymous namespace

std::string_view to_string(Gender g) noexcept
{
    using enum Gender;

    switch (g)
    {
    case Uninitialized: return "uninitialized";
    case Male: return g_male;
    case Female: return g_female;
    case Invalid: return "invalid";
    case Unknown: return "unknown";
    };
    return "invalid";
}

std::string Person::get_given_names() const {
    std::ostringstream oss;

    auto given_it = given_names.begin();

    if (given_it != given_names.end()) {
        oss << *given_it;
        ++given_it;
    }

    while (given_it != given_names.end()) {
        oss << " " << *given_it;
        ++given_it;
    }

    return oss.str();
}


std::string Person::get_last_names() const {
    std::ostringstream oss;

    auto last_it = last_names.begin();

    if (last_it != last_names.end()) {
        oss << *last_it;
        ++last_it;
    }

    while (last_it != last_names.end()) {
        oss << " " << *last_it;
        ++last_it;
    }

    return oss.str();
}


std::string Person::get_full_name() const {
    std::string given_names = get_given_names();
    std::string last_names = get_last_names();

    if (!given_names.empty() && !last_names.empty()) {
        std::ostringstream oss;
        oss << last_names << ", " << given_names;
        return oss.str();
    } else if (!given_names.empty()) {
        return given_names;
    } else if (!last_names.empty()) {
        return last_names;
    }

    return "";
}

std::ostream& Person::operator<<(std::ostream& os)
{
    os << "Person{";
    print_state(os);
    os << "}";
    return os;
}

void Person::print_state(std::ostream& os) const
{
    Resource::print_state(os);
    if (gender.has_value())
    {
        os << ", gender: " << gender.value();
    }
}


#if defined(__GNUC__) && !defined(__clang__) && (__GNUC__ < 14)
# error "g++-14 is required to use the std::chrono::parse function"
#endif

std::chrono::year_month_day convert_date(const std::string& raw)
{
    std::chrono::year_month_day result;

    std::istringstream is{raw};
    is >> std::chrono::parse("%F", result);

    // is.fail() will return true when:
    //  * the input format is not valid (%F stands for ISO-8601, so '01-02-2003' in not valid while
    //    '2003-02-01' is valid);
    //  * the successfully parsed input value doesn't represent a valid date, e.g. the month number
    //     is out of range or the day is out of range for given year and month:
    if (is.fail())
    {
        throw common_exception(
            common_exception::error_code::data_format_error,
            fmt::format("The birth date has unexpected format: '{}'", raw));
    }

    return result;
}


void extract_person_birth_date(Person& person, const data_row& row, const std::string& date_bn)
{
    auto date_it = row.find(date_bn);

    if (date_it != row.end())
    {
        person.birth_date = convert_date(date_it->second);
    }
    else
    {
        spdlog::debug(
            "extract_person_birth_date: The '{}' binding not found for person '{}'",
            date_bn, person.get_unique_id());
    }
}


void extract_person_death_date(Person& person, const data_row& row, const std::string& date_bn)
{
    auto date_it = row.find(date_bn);

    if (date_it != row.end())
    {
        person.death_date = convert_date(date_it->second);
    }
    else
    {
        spdlog::debug(
            "extract_person_death_date: The '{}' binding not found for person '{}'",
            date_bn, person.get_unique_id());
    }
}


Gender extract_person_gender(
    const data_row& row, const std::string& gender_type_bn, std::vector<Note>& notes)
{
    auto gender_it = row.find(gender_type_bn);

    if (gender_it != row.end())
    {
        if (gender_it->second == k_gender_uri_male)
        {
            return Gender::Male;
        }
        else if (gender_it->second == k_gender_uri_female)
        {
            return Gender::Female;
        }
        else
        {
            notes.emplace_back(create_invalid_gender_note());
            return Gender::Invalid;
        }
    }
    else
    {
        notes.emplace_back(create_unspecified_gender_note());
        return Gender::Unknown;
    }
}


void extract_person_names(Person& person, const data_table& table) {
    for (data_row row : table) {
        std::string name_type;
        std::string name_value;

        auto type_it = row.find("nameType");
        if (type_it != row.end()) {
            name_type = type_it->second;
        } else {
            spdlog::warn("The data table is missing the expected 'nameType' field");
            continue;
        }

        auto value_it = row.find("nameValue");
        if (value_it != row.end()) {
            name_value = value_it->second;
        } else {
            spdlog::warn("The data table is missing the expected 'nameValue' field");
            continue;
        }

        if (name_type == "http://gedcomx.org/Given") {
            person.given_names.push_back(name_value);
        } else if (name_type == "http://gedcomx.org/Surname") {
            person.last_names.push_back(name_value);
        }
    }
}


nlohmann::json person_to_json(const Person& person)
{
    nlohmann::json result;

    result["unique_path"] = person.get_unique_id();

    if (person.gender == Gender::Male)
    {
        result["gender"] = g_male;
    }
    else if (person.gender == Gender::Female)
    {
        result["gender"] = g_female;
    }

    std::string full_name = person.get_full_name();

    if (!full_name.empty())
    {
        result["name"]["full"] = full_name;

        std::string given_names = person.get_given_names();

        if (!given_names.empty())
        {
            result["name"]["given"] = given_names;
        }

        std::string last_names = person.get_last_names();

        if (!last_names.empty())
        {
            result["name"]["last"] = last_names;
        }
    }

    if (person.birth_date)
    {
        result["birth_date"] = std::format("{:%Y-%m-%d}", person.birth_date.value());
    }

    if (person.death_date)
    {
        result["death_date"] = std::format("{:%Y-%m-%d}", person.death_date.value());
    }

    if (person.father)
    {
        result["father"] = person_to_json(*person.father);
    }

    if (person.mother)
    {
        result["mother"] = person_to_json(*person.mother);
    }

    if (!person.partners.empty())
    {
        result["partners"] = nlohmann::json::array();

        for (const Person::PartnerRelation& relation : person.partners)
        {
            const auto& partner = *relation.partner;
            auto json_partner = person_to_json(partner);
            auto children_it = person.children.find(partner);

            if (children_it != person.children.end())
            {
                json_partner["children"] = nlohmann::json::array();

                for (const auto& child_it : children_it->second)
                {
                    json_partner["children"].push_back(person_to_json(*child_it));
                }
            }

            json_partner["inferred"] = relation.is_inferred;

            result["partners"].push_back(json_partner);
        }
    }

    const auto single_parent_children_it = person.children.find({});

    if (single_parent_children_it != person.children.end())
    {
        result["children"] = nlohmann::json::array();

        for (const auto& child_it : single_parent_children_it->second)
        {
            result["children"].push_back(person_to_json(*child_it));
        }
    }

    if (!person.notes().empty())
    {
        result["notes"] = nlohmann::json::array();

        for (const auto& note : person.notes())
        {
            result["notes"].push_back(common::note_to_json(note));
        }
    }

    return result;
}

nlohmann::json person_list_to_json(const std::vector<std::shared_ptr<Person>>& person_list)
{
    nlohmann::json result;

    for (const auto& person : person_list)
    {
        result.push_back(person_to_json(*person));
    }

    return result;
}

} // namespace common
