#if !defined PERSON_QUERIES_DETAILS_HPP
#define PERSON_QUERIES_DETAILS_HPP

#include "common/person.hpp"
#include "person/queries/common.hpp"


namespace person
{

std::vector<common::Person::PartnerRelation> retrieve_person_partners(
    const common::Person* person, librdf_world* world, librdf_model* model);

} // namespace person


#endif // !defined PERSON_QUERIES_DETAILS_HPP
