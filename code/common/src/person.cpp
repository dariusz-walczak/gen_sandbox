#include "person.hpp"

#include <cassert>

namespace {
    const char* g_male = "male";
    const char* g_female = "female";
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


Person extract_person(const data_row& row) {
    Person result;

    auto gender_it = row.find("genderType");

    if (gender_it != row.end()) {
        if (gender_it->second == g_male) {
            result.gender = Gender::Male;
        } else if (gender_it->second == g_female) {
            result.gender = Gender::Female;
        } else {
            result.gender = Gender::Unknown;
        }
    } else {
        result.gender = Gender::Unknown;
    }

    return result;
}


nlohmann::json person_to_json(const Person& person) {
    nlohmann::json result;

    if (person.gender == Gender::Male) {
        result["gender"] = g_male;
    } else if (person.gender == Gender::Female) {
        result["gender"] = g_female;
    }

    return result;
}
