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
    std::string iri;
    std::optional<Gender> gender;
    std::vector<std::string> given_names;
    std::vector<std::string> last_names;

    std::shared_ptr<Person> mother;
    std::shared_ptr<Person> father;

    std::vector<std::shared_ptr<Person>> partners;
    std::map<std::string, std::vector<std::shared_ptr<Person>>> children;
};

std::string compose_person_iri(const std::string& id);

/* @brief Extract the string representation of the gender from the Redland Node
 *
 * @note This function work as an extraction callback provided to the extract_cb_lut function */
std::string extract_gender_raw(librdf_node* node);

void extract_person_gender(Person& person, const data_row& row, const std::string& gender_type_bn);
void extract_person_names(Person& person, const data_table& table);
/* @brief Extract the person identifier from the person IRI binding in the data row and store it
 *        in the `id` field of the provided `Person` object.
 *
 * Store an empty string if the IRI has unexpected format.
 *
 * @param person The output object to store the extracted identifier in.
 * @param row    The input data row.
 * @param person_iri_bn The name of the input data row binding to extract the person iri from. */
void extract_person_id(Person& person, const data_row& row, const std::string& person_iri_bn);

nlohmann::json person_to_json(const Person& person);

#endif // !defined COMMON__PERSON_HPP
