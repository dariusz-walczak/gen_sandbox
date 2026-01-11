#if !defined COMMON_RESOURCE_UTILS_HPP
#define COMMON_RESOURCE_UTILS_HPP

#include "common/contract.hpp"
#include "common/resource.hpp"


namespace common
{

/** @brief Ask if a resource is described in the model
 *
 *  The resource is described when at least one RDF triple with the @p resource uri in the subject
 *   position exists in the @p model.
 *
 *  @see @ref described_resource "Described Resource"
 *
 *  @param[in] resource The resource to be asked about.
 *  @param[in] world The _Redland RDF Library_ world owning the @p model.
 *  @param[in] model The _Redland RDF Library_ model to query.
 *
 *  @retval true when the resource is described in the model
 *  @retval false when the resource is not described in the model
 *
 *  @throws common_exception
 *      (@ref common::common_exception::error_code::input_contract_error "input_contract_error")
 *      when any of the input parameters are null
 *  @throws common_exception
 *      (@ref common::common_exception::error_code::redland_query_error "redland_query_error")
 *      on the SPARQL query execution error
 */
bool ask_resource_described(
    const common::Resource* resource, librdf_world* world, librdf_model* model);

/** @brief Ask if a resource is described in the model
 *
 *  The resource is described when at least one RDF triple with the @p resource_uri in the subject
 *   position exists in the @p model.
 *
 *  @see @ref described_resource "Described Resource"
 *
 *  @param[in] resource_uri The URI of the resource to be asked about.
 *  @param[in] world The _Redland RDF Library_ world owning the @p model.
 *  @param[in] model The _Redland RDF Library_ model to query.
 *
 *  @retval true when the resource is described in the model
 *  @retval false when the resource is not described in the model
 *
 *  @throws common_exception
 *      (@ref common::common_exception::error_code::input_contract_error "input_contract_error")
 *      when the @p world or @p model parameters are null
 *  @throws common_exception
 *      (@ref common::common_exception::error_code::redland_query_error "redland_query_error")
 *      on the SPARQL query execution error
 */
bool ask_resource_described(
    const std::string_view resource_uri, librdf_world* world, librdf_model* model);

/** @brief Ask if a resource is referenced in the model
 *
 *  The resource is referenced when at least one RDF triple with the @p resource uri in the object
 *   position exists in the @p model.
 *
 *  @see @ref referenced_resource "Referenced Resource"
 *
 *  @param[in] resource The resource to be asked about.
 *  @param[in] world The _Redland RDF Library_ world owning the @p model.
 *  @param[in] model The _Redland RDF Library_ model to query.
 *
 *  @retval true when the resource is referenced in the model
 *  @retval false when the resource is not referenced in the model
 *
 *  @throws common_exception
 *      (@ref common::common_exception::error_code::input_contract_error "input_contract_error")
 *      when any of the input parameters are null
 *  @throws common_exception
 *      (@ref common::common_exception::error_code::redland_query_error "redland_query_error")
 *      on the SPARQL query execution error
 */
bool ask_resource_referenced(
    const common::Resource* resource, librdf_world* world, librdf_model* model);

/** @brief Ask if a resource is referenced in the model
 *
 *  The resource is referenced when at least one RDF triple with the @p resource_uri in the object
 *   position exists in the @p model.
 *
 *  @see @ref referenced_resource "Referenced Resource"
 *
 *  @param[in] resource_uri The URI of the resource to be asked about.
 *  @param[in] world The _Redland RDF Library_ world owning the @p model.
 *  @param[in] model The _Redland RDF Library_ model to query.
 *
 *  @retval true when the resource is described in the model
 *  @retval false when the resource is not described in the model
 *
 *  @throws common_exception
 *      (@ref common::common_exception::error_code::input_contract_error "input_contract_error")
 *      when the @p world or @p model parameters are null
 *  @throws common_exception
 *      (@ref common::common_exception::error_code::redland_query_error "redland_query_error")
 *      on the SPARQL query execution error
 */
bool ask_resource_referenced(
    const std::string_view resource_uri, librdf_world* world, librdf_model* model);

/** @brief Ask if a resource is untyped in the model
 *
 *  The resource is @ref untyped_resource "untyped" when it is @ref described_resource "described"
 *  but not @ref typed_resource "typed".
 *
 *  @param[in] resource The resource to be asked about.
 *  @param[in] world The _Redland RDF Library_ world owning the @p model.
 *  @param[in] model The _Redland RDF Library_ model to query.
 *
 *  @retval true when the resource is untyped (not described by any type) in the model
 *  @retval false when the resource is described by some type
 *
 *  @throws common_exception
 *      (@ref common::common_exception::error_code::input_contract_error "input_contract_error")
 *      when any of the input parameters are null
 *  @throws common_exception
 *      (@ref common::common_exception::error_code::redland_query_error "redland_query_error")
 *      on the SPARQL query execution error
 */
bool ask_resource_untyped(
    const common::Resource* resource, librdf_world* world, librdf_model* model);

/** @brief Ask if a resource is untyped in the model
 *
 *  The resource is @ref untyped_resource "untyped" when it is @ref described_resource "described"
 *  but not @ref typed_resource "typed".
 *
 *  @param[in] resource_uri The URI of the resource to be asked about.
 *  @param[in] world The _Redland RDF Library_ world owning the @p model.
 *  @param[in] model The _Redland RDF Library_ model to query.
 *
 *  @retval true when the resource is untyped (not described by any type) in the model
 *  @retval false when the resource is described by some type
 *
 *  @throws common_exception
 *      (@ref common::common_exception::error_code::input_contract_error "input_contract_error")
 *      when any of the input parameters are null
 *  @throws common_exception
 *      (@ref common::common_exception::error_code::redland_query_error "redland_query_error")
 *      on the SPARQL query execution error
 */
bool ask_resource_untyped(
    const std::string_view resource_uri, librdf_world* world, librdf_model* model);

/** @brief Ask if a resource is mistyped (incorrectly typed) in the model
 *
 *  The resource is mistyped when it is @ref typed_resource "typed", but the type is different from
 *  what is expected.
 *
 *  @param[in] resource The resource to be asked about.
 *  @param[in] expected_type_uri The expected @p resource type URI.
 *  @param[in] world The _Redland RDF Library_ world owning the @p model.
 *  @param[in] model The _Redland RDF Library_ model to query.
 *
 *  @retval true when the resource is described in the model by some type, but the type is
 *      unexpected
 *  @retval false when the resource is described by the expected type or not typed at all
 *
 *  @throws common_exception
 *      (@ref common::common_exception::error_code::input_contract_error "input_contract_error")
 *      when any of the @p resource, @p world, or @p model parameters are null
 *  @throws common_exception
 *      (@ref common::common_exception::error_code::redland_query_error "redland_query_error")
 *      on the SPARQL query execution error
 */
bool ask_resource_mistyped(
    const common::Resource* resource, const std::string_view expected_type_uri,
    librdf_world* world, librdf_model* model);

/** @brief Ask if a resource is mistyped (incorrectly typed) in the model
 *
 *  The resource is mistyped when it is @ref typed_resource "typed", but the type is different from
 *  what is expected.
 *
 *  @param[in] resource_uri The URI of the resource to be asked about.
 *  @param[in] expected_type_uri The expected @p resource type URI.
 *  @param[in] world The _Redland RDF Library_ world owning the @p model.
 *  @param[in] model The _Redland RDF Library_ model to query.
 *
 *  @retval true when the resource is described in the model by some type, but the type is
 *      unexpected
 *  @retval false when the resource is described by the expected type or not typed at all
 *
 *  @throws common_exception
 *      (@ref common::common_exception::error_code::input_contract_error "input_contract_error")
 *      when any of the @p world, or @p model parameters are null
 *  @throws common_exception
 *      (@ref common::common_exception::error_code::redland_query_error "redland_query_error")
 *      on the SPARQL query execution error
 */
bool ask_resource_mistyped(
    const std::string_view resource_uri, const std::string_view expected_type_uri,
    librdf_world* world, librdf_model* model);

/** @brief Ask if a resource is correctly typed in the model
 *
 *  @param[in] resource The resource to be asked about.
 *  @param[in] expected_type_uri The expected @p resource type URI.
 *  @param[in] world The _Redland RDF Library_ world owning the @p model.
 *  @param[in] model The _Redland RDF Library_ model to query.
 *
 *  @retval true when the resource is described in the model by the expected type
 *  @retval false when the resource is described by an unexpected type or not typed at all
 *
 *  @throws common_exception
 *      (@ref common::common_exception::error_code::input_contract_error "input_contract_error")
 *      when any of the @p resource, @p world, or @p model parameters are null
 *  @throws common_exception
 *      (@ref common::common_exception::error_code::redland_query_error "redland_query_error")
 *      on the SPARQL query execution error
 */
bool ask_resource_typed(
    const common::Resource* resource, const std::string_view expected_type_uri,
    librdf_world* world, librdf_model* model);

/** @brief Ask if a resource is correctly typed in the model
 *
 *  @param[in] resource_uri The URI of the resource to be asked about.
 *  @param[in] expected_type_uri The expected @p resource type URI.
 *  @param[in] world The _Redland RDF Library_ world owning the @p model.
 *  @param[in] model The _Redland RDF Library_ model to query.
 *
 *  @retval true when the resource is described in the model by the expected type
 *  @retval false when the resource is described by an unexpected type or not typed at all
 *
 *  @throws common_exception
 *      (@ref common::common_exception::error_code::input_contract_error "input_contract_error")
 *      when any of the @p world, or @p model parameters are null
 *  @throws common_exception
 *      (@ref common::common_exception::error_code::redland_query_error "redland_query_error")
 *      on the SPARQL query execution error
 */
bool ask_resource_typed(
    const std::string_view resource_uri, const std::string_view expected_type_uri,
    librdf_world* world, librdf_model* model);


template<typename T>
struct is_shared_resource : std::false_type {};

template<typename T>
struct is_shared_resource<std::shared_ptr<T>>
    : std::bool_constant<std::is_base_of_v<common::Resource, T>> {};


/** @brief Extract sequence of URI strings from a sequence of resources
 *
 *  @param resources the input resource sequence
 *  @return sequence of URI strings */
template<typename Sequence>
requires requires
{
    requires std::ranges::input_range<Sequence>;
    requires is_shared_resource<std::ranges::range_value_t<Sequence>>::value;
}
std::vector<std::string> extract_uri_str_seq(Sequence&& resources)
{
    ensure_items_not_null(resources);

    std::vector<std::string> output;

    if constexpr (requires (Sequence& s) { std::ranges::size(s); })
    {
        output.reserve(resources.size()); // TEST with set, c-array and a vector
    }

    std::ranges::transform(
        resources, std::back_inserter(output),
        [](const auto& res) { return res->get_uri_str(); });

    return output;
}

} // namespace common

#endif // !defined COMMON_RESOURCE_UTILS_HPP
