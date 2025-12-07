#include "test/tools/person.hpp"

#include "test/tools/string.hpp"

namespace test::tools
{

common::Person construct_named_person_val(
    const std::string& uri, const std::string& first_name, const std::string& last_name)
{
    common::Person person(uri);
    person.given_names.emplace_back(first_name);
    person.last_names.emplace_back(last_name);
    return person;
}

std::shared_ptr<common::Person> construct_named_person_ptr(
    const std::string& uri, const std::string& first_name, const std::string& last_name)
{
    return std::make_shared<common::Person>(
        construct_named_person_val(uri, first_name, last_name));
}

ComparablePerson to_comparable(const common::Person& person)
{
    return ComparablePerson{person.get_uri_str(), person.get_caption()};
}

std::string to_string(const ComparablePerson& person, std::uint8_t depth) noexcept
{
    const std::string indent = construct_indent(depth);

    std::ostringstream oss;
    oss << indent << "ComparablePerson{"
        << "id=" << person.id << ", "
        << "caption=" << person.caption << "}";
    return oss.str();
}

} // namespace test::tools
