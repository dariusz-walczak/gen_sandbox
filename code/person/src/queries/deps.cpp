#include "person/queries/deps.hpp"

#include <spdlog/spdlog.h>

#include "person/error.hpp"

person_iri_set retrieve_person_iris(librdf_world* world, librdf_model* model)
{
    spdlog::trace("{}: Entry checkpoint", __func__);

    const std::string query = R"(
        PREFIX gx: <http://gedcomx.org/>

        SELECT DISTINCT ?person
        WHERE {
            ?person a gx:Person .
        })";

    spdlog::debug("{}: The query: {}", __func__, query);

    exec_query_result res = exec_query(world, model, query);

    if (!res->success) {
        spdlog::error("{}: The query execution has failed", __func__);

        throw person_exception(
            person_exception::error_code::query_error,
            fmt::format("Failed to execute the '{}' query", __func__));
    }

    const extract_data_table_result data_tuple = extract_data_table(res->results);
    const data_table& data_table = std::get<1>(data_tuple);

    person_iri_set result;

    for (const data_row& row : data_table)
    {
        const std::string bn = { "person" };
        const auto iri_it = row.find(bn);

        if (iri_it == row.end())
        {
            throw person_exception(
                person_exception::error_code::query_error,
                fmt::format(
                    "Failed to find the expected '{}' binding in the query result row", bn));
        }

        result.insert(iri_it->second);
    }

    return result;
}
