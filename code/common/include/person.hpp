#if !defined COMMON__PERSON_HPP
#define COMMON__PERSON_HPP

#include <string>
#include <optional>

#include <redland.h>


enum class Gender {
    Male,
    Female,
    Unknown
};

class Person {
public:
    std::string id;
    std::string get_name() const { return "ugh"; }
    std::optional<Gender> gender;

    std::optional<std::string> _name;
};

std::string extract_gender_raw(librdf_node* node);

#endif // !defined COMMON__PERSON_HPP
