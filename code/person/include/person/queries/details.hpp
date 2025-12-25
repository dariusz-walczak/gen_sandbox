#if !defined PERSON_QUERIES_DETAILS_HPP
#define PERSON_QUERIES_DETAILS_HPP

#include <string_view>

#include "common/person.hpp"
#include "common/note.hpp"
#include "person/queries/common.hpp"


namespace person
{

inline constexpr std::string_view k_inferred_partner_note_id = "INFERRED_PARTNER";
inline constexpr std::string_view k_invalid_inferred_partner_note_id = "INVALID_INFERRED_PARTNER";
inline constexpr std::string_view k_invalid_stated_partner_note_id = "INVALID_STATED_PARTNER";
inline constexpr std::string_view k_multiple_fathers_note_id = "MULTIPLE_FATHERS";
inline constexpr std::string_view k_multiple_mothers_note_id = "MULTIPLE_MOTHERS";

/** @brief Find the father of the given person
 *
 *  Looks up the biological male parent (father) of the @p proband in the @p model and returns a
 *  Person resource object if found.
 *
 *  @param[in] proband The person whose father is being queried.
 *  @param[in] world the Redland RDF Library world owning the @p model.
 *  @param[in] model the Redland RDF Library model to query.
 *  @retval std::shared_ptr<common::Person> representing the father if found
 *  @retval nullptr if no father or more than one father was found
 *
 *  @throws person_exception (input_contract_error) if any of the input parameters is null
 *  @throws common::common_exception (redland_query_error) on the SPARQL query execution error
 */
std::shared_ptr<common::Person> retrieve_person_father(
    const common::Person* proband, librdf_world* world, librdf_model* model,
    std::vector<common::Note>& notes);

/** @brief Find the mother of the given person
 *
 *  Looks up the biological female parent (mother) of the @p proband in the @p model and returns a
 *  Person resource object if found.
 *
 *  @param[in] proband The person whose mother is being queried.
 *  @param[in] world the Redland RDF Library world owning the @p model.
 *  @param[in] model the Redland RDF Library model to query.
 *  @retval std::shared_ptr<common::Person> representing the mother if found
 *  @retval nullptr if no mother or more than one mother was found
 *
 *  @throws person_exception (input_contract_error) if any of the input parameters is null
 *  @throws common::common_exception (redland_query_error) on the SPARQL query execution error
 */
std::shared_ptr<common::Person> retrieve_person_mother(
    const common::Person* proband, librdf_world* world, librdf_model* model,
    std::vector<common::Note>& notes);

std::vector<common::Person::PartnerRelation> retrieve_person_partners(
    const common::Person* proband, librdf_world* world, librdf_model* model,
    std::vector<common::Note>& notes);

} // namespace person


#endif // !defined PERSON_QUERIES_DETAILS_HPP
