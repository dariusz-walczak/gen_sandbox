#include "queries.hpp"


retrieve_result retrieve_person_preferred_name(
    Person& person, const std::string& person_iri, librdf_world* world, librdf_model* model) {

    const std::string query = R"(
        PREFIX gx: <http://gedcomx.org/>
        PREFIX xsd: <http://www.w3.org/2001/XMLSchema#>

        SELECT 
            ?nameType, ?nameValue
        WHERE {
            ?person a gx:Person ;
                gx:name ?name .
            ?name gx:preferred "true"^^xsd:boolean ;
                gx:nameForm ?form .
            ?form gx:part ?part .
            ?part gx:type ?nameType ;
                gx:value ?nameValue .
            FILTER (?person = <)" + person_iri + R"(>)
        })";

    exec_query_result res = exec_query(world, model, query);

    if (!res->success) {
        spdlog::error("The Redland query execution has failed");

        return retrieve_result::QueryError;
    }

    extract_data_table_result data_tuple = extract_data_table(res->results);
    const data_table& data_table = std::get<1>(data_tuple);

    if (data_table.empty()) {
        spdlog::debug("Properly formed preferred names of person {} were not found", person_iri);
        return retrieve_result::NotFound;
    }

    extract_person_names(person, data_table);

    return retrieve_result::Success;
}
