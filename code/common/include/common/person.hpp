#if !defined COMMON_PERSON_HPP
#define COMMON_PERSON_HPP

#include <chrono>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>

#include <nlohmann/json.hpp>
#include <redland.h>

#include "redland_utils.hpp"


enum class Gender : std::uint8_t {
    Uninitialized = 0,
    Male,
    Female,
    Unknown
};


using person_id = std::string;
using person_iri = std::string;


class Person {
public:
    [[nodiscard]] std::string get_given_names() const;
    [[nodiscard]] std::string get_last_names() const;
    [[nodiscard]] std::string get_full_name() const;

    person_id id;
    person_iri iri;
    std::optional<Gender> gender;
    std::vector<std::string> given_names;
    std::vector<std::string> last_names;
    std::optional<std::chrono::year_month_day> birth_date;
    std::optional<std::chrono::year_month_day> death_date;

    std::shared_ptr<Person> mother;
    std::shared_ptr<Person> father;

    std::vector<std::shared_ptr<Person>> partners;
    std::map<std::string, std::vector<std::shared_ptr<Person>>> children;
};

person_iri compose_person_iri(const person_id& id);

/* @brief Extract the string representation of the gender from the Redland Node
 *
 * @note This function work as an extraction callback provided to the extract_cb_lut function */
std::string extract_gender_raw(librdf_node* node);

void extract_person_birth_date(Person& person, const data_row& row, const std::string& date_bn);
void extract_person_death_date(Person& person, const data_row& row, const std::string& date_bn);
void extract_person_gender(Person& person, const data_row& row, const std::string& gender_type_bn);
void extract_person_names(Person& person, const data_table& table);

/* @brief Extract the person identifier from the person IRI binding in the data row and store it
 *        in the `id` field of the provided `Person` object.
 *
 * Throw the common_exception when:
 * * the IRI has unexpected format,
 * * the binding is not found in the row.
 *
 * @param person The output object to store the extracted identifier in.
 * @param row    The input data row.
 * @param person_iri_bn The name of the input data row binding to extract the person iri from. */
void extract_person_id(Person& person, const data_row& row, const std::string& person_iri_bn);

nlohmann::json person_to_json(const Person& person);
nlohmann::json person_list_to_json(const std::vector<Person>& person_list);

#endif // !defined COMMON_PERSON_HPP
