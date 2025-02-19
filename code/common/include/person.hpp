#if !defined COMMON__PERSON_HPP
#define COMMON__PERSON_HPP

#include <string>
#include <optional>

#include <nlohmann/json.hpp>
#include <redland.h>

#include "redland_utils.hpp"

enum class Gender {
    Uninitialized = 0,
    Male,
    Female,
    Unknown
};

class Person {
public:
    std::string id;
    std::string get_name() const { return "ugh"; }
    std::optional<Gender> gender;

    std::optional<std::string> given_name;
    std::optional<std::string> surname;

    std::optional<std::string> _name;
};

/* @brief Extract the string representation of the gender from the Redland Node
 *
 * @note This function work as an extraction callback provided to the extract_cb_lut function */
std::string extract_gender_raw(librdf_node* node);

Person extract_person(const data_row& row);

nlohmann::json person_to_json(const Person& person);

#endif // !defined COMMON__PERSON_HPP
