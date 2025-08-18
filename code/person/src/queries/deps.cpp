#include "person/queries/deps.hpp"

#include <spdlog/spdlog.h>

#include "person/error.hpp"

common::resource_set retrieve_person_iris(librdf_world* world, librdf_model* model)
{
    spdlog::trace("{}: Entry checkpoint", __func__);

    const std::string query = R"(
        PREFIX gx: <http://gedcomx.org/>

        SELECT DISTINCT ?person
        WHERE {
            ?person a gx:Person .
        }
        ORDER BY ASC(?person))";

    spdlog::debug("{}: The query: {}", __func__, query);

    common::exec_query_result res = common::exec_query(world, model, query);

    if (!res->success) {
        spdlog::error("{}: The query execution has failed", __func__);

        throw person_exception(
            person_exception::error_code::query_error,
            fmt::format("Failed to execute the '{}' query", __func__));
    }

    const common::extract_data_table_result data_tuple = common::extract_data_table(res->results);
    const common::data_table& data_table = std::get<1>(data_tuple);

    common::resource_set result;

    for (const common::data_row& row : data_table)
    {
        result.insert(common::extract_resource(row, "person")); // throws common_exception
    }

    return result;
}

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

    common::exec_query_result res = common::exec_query(world, model, query);

    if (!res->success) {
        spdlog::error("{}: The query execution has failed", __func__);

        throw person_exception(
            person_exception::error_code::query_error,
            fmt::format("Failed to execute the '{}' query", __func__));
    }

    const common::extract_data_table_result data_tuple = common::extract_data_table(res->results);
    return std::get<1>(data_tuple);
}

bool ask_resource_referenced(
    librdf_world* world, librdf_model* model, const std::string& resource_iri)
{
    spdlog::trace("{}: Entry checkpoint", __func__);

    const std::string query = R"(
        ASK WHERE {
            <)" + resource_iri + R"(> ?predicate ?object
        })";

    spdlog::debug("{}: The query: {}", __func__, query);

    common::exec_query_result result = common::exec_query(world, model, query);

    if (!result->success)
    {
        spdlog::error("{}: The query execution has failed", __func__);

        throw person_exception(
            person_exception::error_code::query_error,
            fmt::format(
                "Failed to execute the '{}' query", __func__));
    }

    bool response = common::extract_boolean_result(result->results);

    spdlog::debug("{}: The ask query result is '{}'", __func__, response ? "true" : "false");

    return response;
}
