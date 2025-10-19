#include "test/tools/person.hpp"

namespace test::tools
{

std::shared_ptr<common::Person> construct_named_person(
    const std::string& uri, const std::string& first_name, const std::string& last_name)
{
    std::shared_ptr<common::Person> person = std::make_shared<common::Person>(uri);
    person->given_names.emplace_back(first_name);
    person->last_names.emplace_back(last_name);
    return person;
}

} // namespace test::tools
