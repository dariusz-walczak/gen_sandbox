#if !defined COMMON_RESOURCE_UTILS_HPP
#define COMMON_RESOURCE_UTILS_HPP

#include "common/contract.hpp"
#include "common/resource.hpp"


namespace common
{

bool ask_resource_described(
    const common::Resource* resource, librdf_world* world, librdf_model* model);

bool ask_resource_described(
    const std::string_view resource_uri, librdf_world* world, librdf_model* model);


bool ask_resource_referenced(
    const common::Resource* resource, librdf_world* world, librdf_model* model);

bool ask_resource_referenced(
    const std::string_view resource_uri, librdf_world* world, librdf_model* model);


bool ask_resource_untyped(
    const common::Resource* resource, librdf_world* world, librdf_model* model);

bool ask_resource_untyped(
    const std::string_view resource_uri, librdf_world* world, librdf_model* model);


bool ask_resource_mistyped(
    const common::Resource* resource, const std::string_view expected_type_uri,
    librdf_world* world, librdf_model* model);

bool ask_resource_mistyped(
    const std::string_view resource_uri, const std::string_view expected_type_uri,
    librdf_world* world, librdf_model* model);


bool ask_resource_typed(
    const common::Resource* resource, const std::string_view expected_type_uri,
    librdf_world* world, librdf_model* model);

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
