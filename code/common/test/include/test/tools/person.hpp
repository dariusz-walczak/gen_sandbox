#if !defined TEST_TOOLS_PERSON_HPP
#define TEST_TOOLS_PERSON_HPP

#include <memory>
#include <string>

#include "common/person.hpp"

namespace test::tools
{

std::shared_ptr<common::Person> construct_named_person(
    const std::string& uri, const std::string& first_name, const std::string& last_name);

} // namespace test::tools

#endif // !defined TEST_TOOLS_PERSON_HPP
