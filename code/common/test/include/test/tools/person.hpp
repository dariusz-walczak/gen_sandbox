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

} // namespace test::tools

#endif // !defined TEST_TOOLS_PERSON_HPP
