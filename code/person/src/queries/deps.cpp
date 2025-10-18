#include "person/queries/deps.hpp"

#include <spdlog/spdlog.h>

#include "person/error.hpp"

namespace person
{

common::data_table retrieve_related_persons(librdf_world* world, librdf_model* model)
{
    spdlog::trace("{}: Entry checkpoint", __func__);

    const std::string query = R"(
        PREFIX gx: <http://gedcomx.org/>

        SELECT DISTINCT ?person1 ?person2
        WHERE {
            ?relationship a gx:Relationship ;
                gx:person1 ?person1 ;
                gx:person2 ?person2 .
        })";

    spdlog::debug("{}: The query: {}", __func__, query);

    common::exec_query_result res = common::exec_query(world, model, query, __func__);

    const common::extract_data_table_result data_tuple = common::extract_data_table(res->results);
    return std::get<1>(data_tuple);
}

bool ask_resource_referenced(
    librdf_world* world, librdf_model* model, const std::string& resource_uri)
{
    spdlog::trace("{}: Entry checkpoint", __func__);

    const std::string query = R"(
        ASK WHERE {
            <)" + resource_uri + R"(> ?predicate ?object
        })";

    spdlog::debug("{}: The query: {}", __func__, query);

    common::exec_query_result result = common::exec_query(world, model, query, __func__);
    bool response = common::extract_boolean_result(result->results);

    spdlog::debug("{}: The ask query result is '{}'", __func__, response ? "true" : "false");

    return response;
}

} // namespace person
