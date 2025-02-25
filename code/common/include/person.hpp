#if !defined COMMON__PERSON_HPP
#define COMMON__PERSON_HPP

#include <memory>
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
    std::string get_given_names() const;
    std::string get_last_names() const;
    std::string get_full_name() const;

    std::string id;
    std::optional<Gender> gender;
    std::vector<std::string> given_names;
    std::vector<std::string> last_names;

    std::shared_ptr<Person> mother;
    std::shared_ptr<Person> father;

    std::vector<std::shared_ptr<Person>> partners;
    std::map<std::string, std::vector<std::shared_ptr<Person>>> children;
};

/* @brief Extract the string representation of the gender from the Redland Node
 *
 * @note This function work as an extraction callback provided to the extract_cb_lut function */
std::string extract_gender_raw(librdf_node* node);

void extract_person_gender(Person& person, const data_row& row, const std::string& gender_type_bn);
void extract_person_names(Person& person, const data_table& table);

nlohmann::json person_to_json(const Person& person);

#endif // !defined COMMON__PERSON_HPP
