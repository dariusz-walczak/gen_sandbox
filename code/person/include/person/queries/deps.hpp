#if !defined PERSON_QUERIES_DEPS_HPP
#define PERSON_QUERIES_DEPS_HPP

#include <redland.h>

#include "person/queries/common.hpp"
#include "common/resource.hpp"

/**
 *
 * @throw common_exception
 */
common::resource_set retrieve_person_iris(librdf_world* world, librdf_model* model);

bool ask_resource_referenced(
    librdf_world* world, librdf_model* model, const std::string& resource_iri);
bool ask_resource_referenced(
    librdf_world* world, librdf_model* model, const common::Resource& resource);

#endif // !defined PERSON_QUERIES_DEPS_HPP
