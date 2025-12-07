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

/** @brief Query caption data of the specified person resource
 *
 *  The caption data is the data needed by the common::Person::get_caption method.
 *
 *  Construct the @ref common::Person resource. The resource may or may not exist.
 *
 *  @param person_uri uri of the queried resource
 *  @param world the librdf world data (expected non-null)
 *  @param model the librdf model data (expected non-null)
 *
 *  @return the resource if found
 *  @return nullptr if the resource is not found
 *
 *  @throws common::common_exception (redland_query_error) on an unexpected query execution error */
std::shared_ptr<common::Person> retrieve_person_caption_data_opt(
    const std::string& person_uri, librdf_world* world, librdf_model* model);

/** @brief Query caption data of the specified person resource
 *
 *  Construct the @ref common::Person resource. The resource is expected to exist.
 *
 *  @param person_uri uri of the queried resource
 *  @param world the librdf world data (expected non-null)
 *  @param model the librdf model data (expected non-null)
 *
 *  @return the resource
 *  @throws common::common_exception (redland_query_error) on an unexpected query execution error
 *  @throws person_exception (resource_not_found) on the resource not found */
std::shared_ptr<common::Person> retrieve_person_caption_data_req(
    const std::string& person_uri, librdf_world* world, librdf_model* model);

/** @brief Query caption data of the specified person resources
 *
 *  Construct a sequence of the @ref common::Person resources. All the resources are expected to
 *   exist
 *
 *  @param person_uri_seq sequence of the person identifiers
 *  @param world the librdf world data (expected non-null)
 *  @param model the librdf model data (expected non-null)
 *
 *  @throws common::common_exception (redland_query_error) on an unexpected query execution error
 *  @throws person::person_exception (resource_not_found) on any of the person resources not found
 */
std::vector<std::shared_ptr<common::Person>> retrieve_person_caption_data_seq_req(
    const std::vector<std::string>& person_uri_seq, librdf_world* world, librdf_model* model);

/** @brief Query base data of the specified person resource
 *
 *  Construct the @ref common::Person resource. The resource may or may not exist.
 *
 *  @return the resource if found
 *  @return nullptr if the resource is not found
 *
 *  @throws common::common_exception (redland_query_error) on an unexpected query execution error */
std::shared_ptr<common::Person> retrieve_person_base_data_opt(
    const std::string& person_uri, librdf_world* world, librdf_model* model);

/** @brief Query base data of the specified person resource
 *
 *  Construct the @ref common::Person resource. The resource is expected to exist.
 *
 *  @return the resource
 *  @throws common::common_exception (redland_query_error) on an unexpected query execution error
 *  @throws person_exception (resource_not_found) on the resource not found */
std::shared_ptr<common::Person> retrieve_person_base_data_req(
    const std::string& person_uri, librdf_world* world, librdf_model* model);

retrieve_result retrieve_person_name(
    common::Person& person, librdf_world* world, librdf_model* model);

retrieve_result retrieve_person_any_name(
    common::Person& person, librdf_world* world, librdf_model* model);

retrieve_result retrieve_person_birth_name(
    common::Person& person, librdf_world* world, librdf_model* model);

retrieve_result retrieve_person_preferred_name(
    common::Person& person, librdf_world* world, librdf_model* model);

retrieve_result retrieve_person_children(
    common::Person& person, librdf_world* world, librdf_model* model);

/**
 *
 * @throw common_exception
 */
common::resource_set retrieve_person_uris(librdf_world* world, librdf_model* model);

std::vector<std::shared_ptr<common::Person>> retrieve_person_list(
    librdf_world* world, librdf_model* model);

} // namespace person

/** @brief Formatter for the retrieve_result (spdlog requires it) */
template <> struct fmt::formatter<person::retrieve_result>: formatter<string_view>
{
    auto format(person::retrieve_result res, format_context& ctx) const
        -> format_context::iterator;
};

#endif // !defined PERSON_QUERIES_COMMON_HPP
