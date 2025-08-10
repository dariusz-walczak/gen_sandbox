#if !defined PERSON_QUERIES_DEPS_HPP
#define PERSON_QUERIES_DEPS_HPP

#include <memory>

#include <redland.h>

#include "common/resource.hpp"
#include "person/queries/common.hpp"

/**
 *
 * @throw common_exception
 */
common::resource_set retrieve_person_iris(librdf_world* world, librdf_model* model);

bool ask_resource_referenced(
    librdf_world* world, librdf_model* model, const std::string& resource_iri);

inline bool ask_resource_referenced(
    librdf_world* world, librdf_model* model, const common::Resource& resource)
{
    return ask_resource_referenced(world, model, resource.get_iri().buffer());
}

inline bool ask_resource_referenced(
    librdf_world* world, librdf_model* model, const std::shared_ptr<common::Resource>& resource)
{
    return ask_resource_referenced(world, model, resource->get_iri().buffer());
}

#endif // !defined PERSON_QUERIES_DEPS_HPP
