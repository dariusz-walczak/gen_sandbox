#if !defined PERSON_QUERIES_DETAILS_HPP
#define PERSON_QUERIES_DETAILS_HPP

#include <string_view>

#include "common/person.hpp"
#include "common/note.hpp"
#include "person/queries/common.hpp"


namespace person
{

inline constexpr std::string_view k_inferred_partner_note_id = "INFERRED_PARTNER";

/** @brief Find the father of the given person
 *
 *  Looks up the biological male parent (father) of the @p proband in the @p model and returns a
 *  Person resource object if found.
 *
 *  @param[in] proband The person whose father is being queried.
 *  @param[in] world the Redland RDF Library world owning the @p model.
 *  @param[in] model the Redland RDF Library model to query.
 *  @retval std::shared_ptr<common::Person> representing the father if found
 *  @retval nullptr if the father was not found
 *
 *  @throws person_exception (input_contract_error) if any of the input parameters is null
 *  @throws common::common_exception (redland_query_error) on the SPARQL query execution error
 *  @throws person_exception (multiple_resources_found) if more than one father is found for the
 *      @p proband
 */
std::shared_ptr<common::Person> retrieve_person_father_opt(
    const common::Person* proband, librdf_world* world, librdf_model* model);

/** @brief Find the mother of the given person
 *
 *  Looks up the biological female parent (mother) of the @p proband in the @p model and returns a
 *  Person resource object if found.
 *
 *  @param[in] proband The person whose mother is being queried.
 *  @param[in] world the Redland RDF Library world owning the @p model.
 *  @param[in] model the Redland RDF Library model to query.
 *  @retval std::shared_ptr<common::Person> representing the mother if found
 *  @retval nullptr if the mother was not found
 *
 *  @throws person_exception (input_contract_error) if any of the input parameters is null
 *  @throws common::common_exception (redland_query_error) on the SPARQL query execution error
 *  @throws person_exception (multiple_resources_found) if more than one mother is found for the
 *      @p proband
 */
std::shared_ptr<common::Person> retrieve_person_mother_opt(
    const common::Person* proband, librdf_world* world, librdf_model* model);

std::vector<common::Person::PartnerRelation> retrieve_person_partners(
    const common::Person* proband, librdf_world* world, librdf_model* model,
    std::vector<common::Note>& notes);

} // namespace person


#endif // !defined PERSON_QUERIES_DETAILS_HPP
