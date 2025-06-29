#if !defined PERSON_QUERIES_DEPS_HPP
#define PERSON_QUERIES_DEPS_HPP

#include <redland.h>

#include "person/queries/common.hpp"
#include "common/person.hpp"

using person_iri_set = std::set<person_iri>;

person_iri_set retrieve_person_iris(librdf_world* world, librdf_model* model);


#endif // !defined PERSON_QUERIES_DEPS_HPP
