#if !defined PERSON__QUERIES_HPP
#define PERSON__QUERIES_HPP

#include <redland.h>

#include <spdlog/spdlog.h>

#include "person.hpp"

enum class retrieve_result {
    Uninitialized = 0,
    NotFound,
    Success
};

retrieve_result retrieve_person(
    Person& person, const std::string& person_iri, librdf_world* world, librdf_model* model);

retrieve_result retrieve_person_name(
    Person& person, const std::string& person_iri, librdf_world* world, librdf_model* model);

retrieve_result retrieve_person_any_name(
    Person& person, const std::string& person_iri, librdf_world* world, librdf_model* model);

retrieve_result retrieve_person_birth_name(
    Person& person, const std::string& person_iri, librdf_world* world, librdf_model* model);

retrieve_result retrieve_person_preferred_name(
    Person& person, const std::string& person_iri, librdf_world* world, librdf_model* model);

retrieve_result retrieve_person_relatives(
    Person& person, const std::string& person_iri, librdf_world* world, librdf_model* model);

retrieve_result retrieve_person_parents(
    Person& person, const std::string& person_iri, librdf_world* world, librdf_model* model);

/** @brief Formatter for the retrieve_result (spdlog requires it) */
template <> struct fmt::formatter<retrieve_result>: formatter<string_view> {
  auto format(retrieve_result r, format_context& ctx) const
    -> format_context::iterator;
};

#endif // !defined PERSON__QUERIES_HPP
