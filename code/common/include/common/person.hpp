#if !defined COMMON_PERSON_HPP
#define COMMON_PERSON_HPP

#include <chrono>
#include <cstdint>
#include <memory>
#include <optional>
#include <ostream>
#include <string>

#include <nlohmann/json.hpp>
#include <redland.h>

#include "common/note.hpp"
#include "common/redland_utils.hpp"
#include "common/resource.hpp"

namespace common
{

inline constexpr std::string_view k_invalid_gender_note_id = "INVALID_GENDER";
inline constexpr std::string_view k_unspecified_gender_note_id = "UNSPECIFIED_GENDER";
inline constexpr std::string_view k_gender_uri_male = "http://gedcomx.org/Male";
inline constexpr std::string_view k_gender_uri_female = "http://gedcomx.org/Female";
inline constexpr std::string_view k_person_type_uri = "http://gedcomx.org/Person";

enum class Gender : std::uint8_t
{
    Uninitialized = 0,
    Male,
    Female,
    Invalid,
    Unknown
};

[[nodiscard]] std::string_view to_string(Gender g) noexcept;

inline std::ostream& operator<<(std::ostream& os, Gender g)
{
    os << to_string(g);
    return os;
}

using person_id = std::string;
using person_iri = std::string;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

class Person : public Resource
{
public:
    struct PartnerRelation
    {
        std::shared_ptr<Person> partner;
        bool is_inferred;
    };

    Person(const std::string& uri) : Resource(uri) {}

    [[nodiscard]] std::string get_given_names() const;
    [[nodiscard]] std::string get_last_names() const;
    [[nodiscard]] std::string get_full_name() const;
    [[nodiscard]] std::string get_caption() const override { return get_full_name(); }

    std::optional<Gender> gender;
    std::vector<std::string> given_names;
    std::vector<std::string> last_names;
    std::optional<std::chrono::year_month_day> birth_date;
    std::optional<std::chrono::year_month_day> death_date;

    std::shared_ptr<Person> mother;
    std::shared_ptr<Person> father;

    std::vector<PartnerRelation> partners;
    std::map<Resource, std::vector<std::shared_ptr<Person>>> children;

    std::ostream& operator<<(std::ostream& os);

    void print_state(std::ostream& os) const override;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

void extract_person_birth_date(Person& person, const data_row& row, const std::string& date_bn);
void extract_person_death_date(Person& person, const data_row& row, const std::string& date_bn);
Gender extract_person_gender(
    const data_row& row, const std::string& gender_type_bn, std::vector<Note>& notes);
void extract_person_names(Person& person, const data_table& table);

nlohmann::json person_to_json(const Person& person);
nlohmann::json person_list_to_json(const std::vector<std::shared_ptr<Person>>& person_list);

} // namespace common

#endif // !defined COMMON_PERSON_HPP
