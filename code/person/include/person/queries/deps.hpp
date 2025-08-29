#if !defined PERSON_QUERIES_DEPS_HPP
#define PERSON_QUERIES_DEPS_HPP

#include <memory>

#include <redland.h>

#include "common/resource.hpp"
#include "person/queries/common.hpp"

namespace person
{

common::data_table retrieve_related_persons(librdf_world* world, librdf_model* model);

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

} // namespace person

#endif // !defined PERSON_QUERIES_DEPS_HPP
