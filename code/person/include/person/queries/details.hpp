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
inline constexpr std::string_view k_stubbed_parent_note_id = "STUBBED_PARENT";
inline constexpr std::string_view k_stubbed_proband_note_id = "STUBBED_PROBAND";
inline constexpr std::string_view k_untyped_parent_note_id = "UNTYPED_PARENT";
inline constexpr std::string_view k_untyped_proband_note_id = "UNTYPED_PROBAND";
inline constexpr std::string_view k_mistyped_parent_note_id = "MISTYPED_PARENT";
inline constexpr std::string_view k_mistyped_proband_note_id = "MISTYPED_PROBAND";
inline constexpr std::string_view k_unknown_parent_gender_note_id = "UNKNOWN_PARENT_GENDER";
inline constexpr std::string_view k_unknown_proband_gender_note_id = "UNKNOWN_PROBAND_GENDER";
inline constexpr std::string_view k_invalid_parent_gender_note_id = "INVALID_PARENT_GENDER";
inline constexpr std::string_view k_invalid_proband_gender_note_id = "INVALID_PROBAND_GENDER";

/** @brief Find the father of the given person
 *
 *  Looks up the biological male parent (father) of the @p proband in the @p model and returns a
 *  Person resource object if found.
 *
 *  @param[in] proband The person whose father is being queried.
 *  @param[in] world The _Redland RDF Library_ world object that owns the @p model.
 *  @param[in] model The _Redland RDF Library_ model to query.
 *  @retval std::shared_ptr<common::Person> representing the father if found
 *  @retval nullptr if no father or more than one father was found
 *
 *  @throws person_exception
 *      (@ref person::person_exception::error_code::input_contract_error "input_contract_error")
 *      if any input parameter is null
 *  @throws common::common_exception
 *      (@ref common::common_exception::error_code::redland_query_error "redland_query_error")
 *      on SPARQL query execution error
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
 *  @param[in] world The _Redland RDF Library_ world object that owns the @p model.
 *  @param[in] model The _Redland RDF Library_ model to query.
 *  @retval std::shared_ptr<common::Person> representing the mother if found
 *  @retval nullptr if no mother or more than one mother was found
 *
 *  @throws person_exception
 *      (@ref person::person_exception::error_code::input_contract_error "input_contract_error")
 *      if any input parameter is null
 *  @throws common::common_exception
 *      (@ref common::common_exception::error_code::redland_query_error "redland_query_error")
 *      on SPARQL query execution error
 */
std::shared_ptr<common::Person> retrieve_person_mother(
    const common::Person* proband, librdf_world* world, librdf_model* model,
    std::vector<common::Note>& notes);

/** @brief Retrieve all persons stated to be the proband's parents
 *
 *  Collects every @ref person_resource for which there exists a parent-child relationship in the
 *  @p model where the person is in the parent role and the given @p proband is in the child role.
 *
 *  The function does not filter by resource state (e.g. @ref stubbed_resource "stubbed",
 *  @ref untyped_resource "untyped", etc.) and returns all matches found.
 *
 *  Duplicates are removed. The order of the returned persons is unspecified.
 *
 *  @param[in] proband The person whose parents are being queried.
 *  @param[in] world The _Redland RDF Library_ world object that owns the @p model.
 *  @param[in] model The _Redland RDF Library_ model to query.
 *  @return A sequence of common::Resource objects representing the @p proband parents found in
 *          the @p model.
 *
 *  @note The function returns common::Resource objects rather than common::Person objects because
 *        some of the retrieved resources aren't formally persons (e.g. when they are stubbed,
 *        untyped or mistyped).
 *
 *  @throws person_exception
 *      (@ref person::person_exception::error_code::input_contract_error "input_contract_error")
 *      if any input parameter is null
 *  @throws common::common_exception
 *      (@ref common::common_exception::error_code::redland_query_error "redland_query_error")
 *      on SPARQL query execution error */
std::vector<std::shared_ptr<common::Resource>> retrieve_person_parents(
    const common::Person* proband, librdf_world* world, librdf_model* model);

/** @brief Retrieve validity notes regarding a person's parents
 *
 *  Searches for invalid parents (stubbed, untyped, or mistyped) and regular parents whose
 *  gender is either unspecified or has an invalid value. Creates a validity note for each found
 *  issue.
 *
 *  An invalid parent is a resource that is a:
 *  * @ref stubbed_resource,
 *  * @ref untyped_resource,
 *  * @ref non-person_resource.
 *
 *  A regular parent is a @ref person_resource.
 *
 *  A gender is unspecified when a related Person Resource has no
 *  `(?person gx:gender ?gender)` or no `(?gender gx:type ?type)` triple.
 *
 *  A gender type is invalid when its value is different than `gx:Male` and `gx:Female`.
 *
 *  @param[in] proband The person whose parents are being queried.
 *  @param[in] world The _Redland RDF Library_ world object that owns the @p model.
 *  @param[in] model The _Redland RDF Library_ model to query.
 *  @param[out] notes the validity note sequence to which the created notes are appended.
 *
 *  @throws person_exception
 *      (@ref person::person_exception::error_code::input_contract_error "input_contract_error")
 *      if any input parameter is null
 *  @throws person_exception
 *      (@ref person::person_exception::error_code::input_contract_error "input_contract_error")
 *      if the proband is an unknown resource (not referenced nor described)
 *  @throws common::common_exception
 *      (@ref common::common_exception::error_code::redland_query_error "redland_query_error")
 *      on SPARQL query execution error
 */
void retrieve_person_invalid_parent_notes(
    const common::Person* proband, librdf_world* world, librdf_model* model,
    std::vector<common::Note>& notes);

std::vector<common::Person::PartnerRelation> retrieve_person_partners(
    const common::Person* proband, librdf_world* world, librdf_model* model,
    std::vector<common::Note>& notes);

} // namespace person


#endif // !defined PERSON_QUERIES_DETAILS_HPP
