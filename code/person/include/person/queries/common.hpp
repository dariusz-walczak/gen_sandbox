#if !defined PERSON_QUERIES_COMMON_HPP
#define PERSON_QUERIES_COMMON_HPP

#include <cstdint>

#include <redland.h>
#include <spdlog/spdlog.h>

#include "common/person.hpp"
#include "common/resource.hpp"

namespace person
{

enum class retrieve_result : std::uint8_t
{
    NotFound = 0,
    Success
};

retrieve_result retrieve_person_base_data_opt(
    common::Person& person, const std::string& person_iri,
    librdf_world* world, librdf_model* model);

void retrieve_person_base_data_req(
    common::Person& person, const std::string& person_iri,
    librdf_world* world, librdf_model* model);

retrieve_result retrieve_person_name(
    common::Person& person, const std::string& person_iri,
    librdf_world* world, librdf_model* model);

retrieve_result retrieve_person_any_name(
    common::Person& person, const std::string& person_iri,
    librdf_world* world, librdf_model* model);

retrieve_result retrieve_person_birth_name(
    common::Person& person, const std::string& person_iri,
    librdf_world* world, librdf_model* model);

retrieve_result retrieve_person_preferred_name(
    common::Person& person, const std::string& person_iri,
    librdf_world* world, librdf_model* model);

retrieve_result retrieve_person_relatives(
    common::Person& person, const std::string& person_iri,
    librdf_world* world, librdf_model* model);

retrieve_result retrieve_person_parents(
    common::Person& person, const std::string& person_iri,
    librdf_world* world, librdf_model* model);

retrieve_result retrieve_person_partners(
    common::Person& person, const std::string& person_iri,
    librdf_world* world, librdf_model* model);

retrieve_result retrieve_person_children(
    common::Person& person, const std::string& person_iri,
    librdf_world* world, librdf_model* model);

/**
 *
 * @throw common_exception
 */
common::resource_set retrieve_person_iris(librdf_world* world, librdf_model* model);

std::vector<common::Person> retrieve_person_list(librdf_world* world, librdf_model* model);

} // namespace person

/** @brief Formatter for the retrieve_result (spdlog requires it) */
template <> struct fmt::formatter<person::retrieve_result>: formatter<string_view>
{
    auto format(person::retrieve_result res, format_context& ctx) const
        -> format_context::iterator;
};

#endif // !defined PERSON_QUERIES_COMMON_HPP
