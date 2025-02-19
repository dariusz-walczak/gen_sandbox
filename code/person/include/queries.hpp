#if !defined PERSON__QUERIES_HPP
#define PERSON__QUERIES_HPP

#include <redland.h>

#include "person.hpp"

enum class retrieve_result {
    Uninitialized = 0,
    QueryError,
    NotFound,
    Success
};

retrieve_result retrieve_person_preferred_name(
    Person& person, const std::string& person_iri, librdf_world* world, librdf_model* model);

#endif // !defined PERSON__QUERIES_HPP
