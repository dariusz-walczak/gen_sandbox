#if !defined COMMON_PERSON_HPP
#define COMMON_PERSON_HPP

#include <chrono>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>

#include <nlohmann/json.hpp>
#include <redland.h>

#include "common/redland_utils.hpp"
#include "common/resource.hpp"

namespace common
{

enum class Gender : std::uint8_t {
    Uninitialized = 0,
    Male,
    Female,
    Unknown
};


using person_id = std::string;
using person_iri = std::string;


class Person : public Resource
{
public:
    Person(const std::string& uri) : Resource(uri) {}

    [[nodiscard]] std::string get_given_names() const;
    [[nodiscard]] std::string get_last_names() const;
    [[nodiscard]] std::string get_full_name() const;

    std::optional<Gender> gender;
    std::vector<std::string> given_names;
    std::vector<std::string> last_names;
    std::optional<std::chrono::year_month_day> birth_date;
    std::optional<std::chrono::year_month_day> death_date;

    std::shared_ptr<Person> mother;
    std::shared_ptr<Person> father;

    std::vector<std::shared_ptr<Person>> partners;
    std::map<Resource, std::vector<std::shared_ptr<Person>>> children;
};

/* @brief Extract the string representation of the gender from the Redland Node
 *
 * @note This function work as an extraction callback provided to the extract_cb_lut function */
std::string extract_gender_raw(librdf_node* node);

void extract_person_birth_date(Person& person, const data_row& row, const std::string& date_bn);
void extract_person_death_date(Person& person, const data_row& row, const std::string& date_bn);
void extract_person_gender(Person& person, const data_row& row, const std::string& gender_type_bn);
void extract_person_names(Person& person, const data_table& table);

nlohmann::json person_to_json(const Person& person);
nlohmann::json person_list_to_json(const std::vector<std::shared_ptr<Person>>& person_list);

} // namespace common

#endif // !defined COMMON_PERSON_HPP
