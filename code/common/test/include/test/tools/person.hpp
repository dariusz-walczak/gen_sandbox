#if !defined TEST_TOOLS_PERSON_HPP
#define TEST_TOOLS_PERSON_HPP

#include <memory>
#include <string>

#include "common/person.hpp"

namespace test::tools
{

common::Person construct_named_person_val(
    const std::string& uri, const std::string& first_name, const std::string& last_name);

std::shared_ptr<common::Person> construct_named_person_ptr(
    const std::string& uri, const std::string& first_name, const std::string& last_name);


struct ComparablePerson
{
    std::string id;
    std::string caption;
};

inline bool operator==(const ComparablePerson& lhs, const ComparablePerson& rhs)
{
    return ((lhs.id == rhs.id) && (lhs.caption == rhs.caption));
}

[[nodiscard]] ComparablePerson to_comparable(const common::Person& person);
[[nodiscard]] std::string to_string(const ComparablePerson& person, std::uint8_t depth=0) noexcept;

inline void PrintTo(const ComparablePerson& person, std::ostream* os)
{
    *os << to_string(person);
}

} // namespace test::tools

#endif // !defined TEST_TOOLS_PERSON_HPP
