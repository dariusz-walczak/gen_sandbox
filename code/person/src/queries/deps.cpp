#include "person/queries/deps.hpp"

#include <spdlog/spdlog.h>

#include "person/error.hpp"

namespace person
{

/**
 * Retrieve all distinct pairs of gx:Person resources that are related.
 *
 * A pair (x,y) is returned if and only if:
 *  - x and y appear together in the same gx:Relationship resource (any subtype), or
 *  - x and y are inferred partners (they share at least one common child).
 *
 * Uniqueness and ordering:
 *  - Pairs are *unordered*: (x,y) and (y,x) are considered the same relation.
 *  - Each unordered pair is returned exactly once.
 *  - Self-pairs are excluded (x ≠ y).
 *
 * @return Sequence of unique, canonicalized person pairs (a,b).
 */
common::data_table retrieve_related_persons(librdf_world* world, librdf_model* model)
{
    spdlog::trace("{}: Entry checkpoint", __func__);

    const std::string query = R"(
        PREFIX gx: <http://gedcomx.org/>

        SELECT DISTINCT ?person1 ?person2
        WHERE
        {
            {
                ?relationship a gx:Relationship ;
                    gx:person1 ?candidate1 ;
                    gx:person2 ?candidate2 .
                ?candidate1 a gx:Person .
                ?candidate2 a gx:Person .
            }
            UNION
            {
                ?rel1 a gx:Relationship ;
                    gx:person1 ?candidate1 ;
                    gx:person2 ?child ;
                    gx:type gx:ParentChild .
                ?rel2 a gx:Relationship ;
                    gx:person1 ?candidate2 ;
                    gx:person2 ?child ;
                    gx:type gx:ParentChild .
                ?candidate1 a gx:Person .
                ?candidate2 a gx:Person .
                ?child a gx:Person .
            }
            FILTER (!sameTerm(?candidate1, ?candidate2))
            BIND(IF(STR(?candidate1) < STR(?candidate2), ?candidate1, ?candidate2) AS ?person1)
            BIND(IF(STR(?candidate1) < STR(?candidate2), ?candidate2, ?candidate1) AS ?person2)
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
