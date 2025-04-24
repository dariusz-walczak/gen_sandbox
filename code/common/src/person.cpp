#include "common/person.hpp"

#include <cassert>

#include <spdlog/spdlog.h>

#include "common/common_exception.hpp"

namespace {
    const char* g_male = "male";
    const char* g_female = "female";
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


std::string extract_gender_raw(librdf_node* node) {
    assert(node);

    librdf_uri* uri = librdf_node_get_uri(node);
    std::string_view raw_value(
        reinterpret_cast<char*>(
            librdf_uri_as_string(uri)));

    if (raw_value == "http://gedcomx.org/Male") {
        return g_male;
    } else if (raw_value == "http://gedcomx.org/Female") {
        return g_female;
    } else {
        return "";
    }
}


void extract_person_gender(
    Person& person, const data_row& row, const std::string& gender_type_bn)
{

    auto gender_it = row.find(gender_type_bn);

    if (gender_it != row.end()) {
        if (gender_it->second == g_male) {
            person.gender = Gender::Male;
        } else if (gender_it->second == g_female) {
            person.gender = Gender::Female;
        } else {
            person.gender = Gender::Unknown;
        }
    } else {
        person.gender = Gender::Unknown;
    }
}

namespace
{
    const std::string g_person_prefix = { "http://example.org/" };
}

std::string compose_person_iri(const std::string& id)
{
    return g_person_prefix + id;
}


void extract_person_id(Person& person, const data_row& row, const std::string& person_iri_bn)
{
    auto iri_it = row.find(person_iri_bn);

    if (iri_it != row.end()) {
        if (iri_it->second.starts_with(g_person_prefix))
        {
            person.id = iri_it->second.substr(g_person_prefix.size());
            person.iri = iri_it->second;
        }
        else
        {
            throw common_exception(
                common_exception::error_code::data_format_error,
                fmt::format("The person iri has unexpected format: '{}'", iri_it->second));
        }
    }
    else
    {
        throw common_exception(
            common_exception::error_code::binding_not_found,
            fmt::format("The data row is missing the '{}' binding", person_iri_bn));
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


nlohmann::json person_to_json(const Person& person) {
    nlohmann::json result;

    if (person.gender == Gender::Male) {
        result["gender"] = g_male;
    } else if (person.gender == Gender::Female) {
        result["gender"] = g_female;
    }

    std::string full_name = person.get_full_name();

    if (!full_name.empty()) {
        result["name"]["full"] = full_name;

        std::string given_names = person.get_given_names();

        if (!given_names.empty()) {
            result["name"]["given"] = given_names;
        }

        std::string last_names = person.get_last_names();

        if (!last_names.empty()) {
            result["name"]["last"] = last_names;
        }
    }

    if (person.father) {
        result["father"] = person_to_json(*person.father);
    }

    if (person.mother) {
        result["mother"] = person_to_json(*person.mother);
    }

    if (!person.partners.empty()) {
        result["partners"] = nlohmann::json::array();

        for (auto partner : person.partners) {
            auto json_partner = person_to_json(*partner);
            auto children_it = person.children.find(partner->id);

            if (children_it != person.children.end())
            {
                json_partner["children"] = nlohmann::json::array();

                for (auto child_it : children_it->second)
                {
                    json_partner["children"].push_back(person_to_json(*child_it));
                }
            }

            result["partners"].push_back(json_partner);
        }
    }

    const auto single_parent_children_it = person.children.find("");

    if (single_parent_children_it != person.children.end())
    {
        result["children"] = nlohmann::json::array();

        for (auto child_it : single_parent_children_it->second)
        {
            result["children"].push_back(person_to_json(*child_it));
        }
    }

    return result;
}
