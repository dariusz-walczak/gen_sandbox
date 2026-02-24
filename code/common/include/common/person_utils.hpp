#if !defined COMMON_PERSON_UTILS_HPP
#define COMMON_PERSON_UTILS_HPP

#include "common/contract.hpp"
#include "common/person.hpp"

namespace common
{

/** @brief Ask if @ref person_gender is @ref unknown_person_gender "Unknown"
 *
 *  @param[in] person The person to be asked about.
 *  @param[in] world The _Redland RDF Library_ world owning the @p model.
 *  @param[in] model The _Redland RDF Library_ model to query.
 *
 *  @retval true when the _%Person Gender_ is _Unknown_ in the @p model.
 *  @retval false when the _%Person Gender_ is %ref known_person_gender "Known" in the @p model.
 *
 *  @throws common_exception
 *      (@ref common::common_exception::error_code::input_contract_error "input_contract_error")
 *      if any input parameter is null
 *  @throws common_exception
 *      (@ref common::common_exception::error_code::redland_query_error "redland_query_error")
 *      on SPARQL query execution error
 */
bool ask_person_gender_unknown(
    const common::Resource* person, librdf_world* world, librdf_model* model);

/** @brief Ask if @ref person_gender is @ref unknown_person_gender "Unknown"
 *
 *  @param[in] person The URI of the person to be asked about.
 *  @param[in] world The _Redland RDF Library_ world owning the @p model.
 *  @param[in] model The _Redland RDF Library_ model to query.
 *
 *  @retval true when the _%Person Gender_ is _Unknown_ in the @p model.
 *  @retval false when the _%Person Gender_ is %ref known_person_gender "Known" in the @p model.
 *
 *  @throws common_exception
 *      (@ref common::common_exception::error_code::input_contract_error "input_contract_error")
 *      if any input parameter is null
 *  @throws common_exception
 *      (@ref common::common_exception::error_code::redland_query_error "redland_query_error")
 *      on SPARQL query execution error
 */
bool ask_person_gender_unknown(
    const std::string_view person_uri, librdf_world* world, librdf_model* model);

} // namespace common

#endif // !defined COMMON_PERSON_UTILS_HPP
