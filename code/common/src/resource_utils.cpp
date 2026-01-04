#include "common/resource_utils.hpp"


namespace common
{

bool retrieve_resource_described_flag(
    const common::Resource* resource, librdf_world* world, librdf_model* model)
{
    if (!resource)
    {
        throw common_exception(
            common_exception::error_code::input_contract_error,
            fmt::format(
                "Precondition failure: resource={} must satisfy !nullptr", fmt::ptr(resource)));
    }

    if (!world)
    {
        throw common_exception(
            common_exception::error_code::input_contract_error,
            fmt::format(
                "Precondition failure: world={} must satisfy !nullptr", fmt::ptr(world)));
    }

    if (!model)
    {
        throw common_exception(
            common_exception::error_code::input_contract_error,
            fmt::format(
                "Precondition failure: model={} must satisfy !nullptr", fmt::ptr(model)));
    }

    /* Note:
     *     The `ASK` query worked correctly for all cases with Redland/Rasqal as opposed to a
     *     `SELECT` query, which was problematic to construct due to the missing SPARQL 1.1
     *     features in Rasqal (`EXISTS`, `NOT EXISTS`, `VALUES`, `BIND` scope) – see the
     *     "Unimplemented parts of SPARQL 1.1" section of the 0.9.33 release notes at
     *     https://librdf.org/rasqal/. The `ASK` query also results in simpler C++ code (no need to
     *     construct `common::data_table`). */

    const std::string query = R"(
        ASK
        {
            <)" + resource->get_uri_str() + R"(> ?p ?o
        }
    )";

    const char* query_id = "retrieve resource described flag";

    spdlog::debug("{}: The '{}' query: {}", __func__, query_id, query);

    common::exec_query_result res = common::exec_query(world, model, query, query_id);

    bool response = common::extract_boolean_result(res->results);

    spdlog::debug("{}: The ask query result is '{}'", __func__, response ? "true" : "false");

    return response;
}

} // namespace common
