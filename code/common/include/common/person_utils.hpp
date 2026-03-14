#if !defined COMMON_PERSON_UTILS_HPP
#define COMMON_PERSON_UTILS_HPP

#include "common/contract.hpp"
#include "common/person.hpp"

namespace common
{

/** @brief Ask if @ref person_gender is @ref unknown_person_gender "Unknown"
 *
 *  @param[in] person The person to be asked about.
 *  @param[in] world The _Redland RDF Library_ world object that owns the @p model.
 *  @param[in] model The _Redland RDF Library_ model to query.
 *
 *  @retval true when the _%Person Gender_ is _Unknown_ in the @p model.
 *  @retval false when the _%Person Gender_ is %ref known_person_gender "Known" in the @p model.
 *
 *  @throws common_exception
 *      (@ref common::common_exception::error_code::input_contract_error "input_contract_error")
 *      if any input parameter is @c nullptr
 *  @throws common_exception
 *      (@ref common::common_exception::error_code::redland_query_error "redland_query_error")
 *      on SPARQL query execution error
 */
bool ask_person_gender_unknown(
    const common::Resource* person, librdf_world* world, librdf_model* model);

/** @brief Ask if @ref person_gender is @ref unknown_person_gender "Unknown"
 *
 *  @param[in] person_uri The URI of the person to be asked about.
 *  @param[in] world The _Redland RDF Library_ world object that owns the @p model.
 *  @param[in] model The _Redland RDF Library_ model to query.
 *
 *  @retval true when the _%Person Gender_ is _Unknown_ in the @p model.
 *  @retval false when the _%Person Gender_ is %ref known_person_gender "Known" in the @p model.
 *
 *  @throws common_exception
 *      (@ref common::common_exception::error_code::input_contract_error "input_contract_error")
 *      if either @p world or @p model is @c nullptr
 *  @throws common_exception
 *      (@ref common::common_exception::error_code::redland_query_error "redland_query_error")
 *      on SPARQL query execution error
 */
bool ask_person_gender_unknown(
    const std::string_view person_uri, librdf_world* world, librdf_model* model);

/** @brief Ask if @ref person_gender is @ref invalid_person_gender "Invalid"
 *
 *  @param[in] person The person to be asked about.
 *  @param[in] world The _Redland RDF Library_ world object that owns the @p model.
 *  @param[in] model The _Redland RDF Library_ model to query.
 *
 *  @retval true when the _%Person Gender_ is _Invalid_ in the @p model.
 *  @retval false when the _%Person Gender_ is @ref valid_person_gender "Valid" in the @p model.
 *
 *  @throws common_exception
 *      (@ref common::common_exception::error_code::input_contract_error "input_contract_error")
 *      if any input parameter is @c nullptr
 *  @throws common_exception
 *      (@ref common::common_exception::error_code::redland_query_error "redland_query_error")
 *      on SPARQL query execution error
 */
bool ask_person_gender_invalid(
    const common::Resource* person, librdf_world* world, librdf_model* model);

/** @brief Ask if @ref person_gender is @ref invalid_person_gender "Invalid"
 *
 *  @param[in] person_uri The URI of the person to be asked about.
 *  @param[in] world The _Redland RDF Library_ world object that owns the @p model.
 *  @param[in] model The _Redland RDF Library_ model to query.
 *
 *  @retval true when the _%Person Gender_ is _Invalid_ in the @p model.
 *  @retval false when the _%Person Gender_ is @ref valid_person_gender "Valid" in the @p model.
 *
 *  @throws common_exception
 *      (@ref common::common_exception::error_code::input_contract_error "input_contract_error")
 *      if either @p world or @p model is @c nullptr
 *  @throws common_exception
 *      (@ref common::common_exception::error_code::redland_query_error "redland_query_error")
 *      on SPARQL query execution error
 */
bool ask_person_gender_invalid(
    const std::string_view person_uri, librdf_world* world, librdf_model* model);

} // namespace common

#endif // !defined COMMON_PERSON_UTILS_HPP
