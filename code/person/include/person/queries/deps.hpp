#if !defined PERSON_QUERIES_DEPS_HPP
#define PERSON_QUERIES_DEPS_HPP

#include <memory>

#include <redland.h>

#include "common/resource.hpp"
#include "person/queries/common.hpp"

namespace person
{

common::data_table retrieve_related_persons(librdf_world* world, librdf_model* model);


} // namespace person

#endif // !defined PERSON_QUERIES_DEPS_HPP
