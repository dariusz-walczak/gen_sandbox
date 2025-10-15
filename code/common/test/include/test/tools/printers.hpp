#if !defined TEST_TOOLS_PRINTERS_HPP
#define TEST_TOOLS_PRINTERS_HPP

#include "common/person.hpp"
#include "common/resource.hpp"

namespace common
{

inline void PrintTo(const Person& person, std::ostream* os) {
    *os << "Person{";
    person.print_state(*os);
    *os << "}";
};

inline void PrintTo(const Resource& resource, std::ostream* os) {
    *os << "Resource{";
    resource.print_state(*os);
    *os << "}";
};

} // namespace common

#endif // !defined TEST_TOOLS_PRINTERS_HPP
