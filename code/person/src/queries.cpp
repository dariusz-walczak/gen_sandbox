#include "queries.hpp"


retrieve_result retrieve_person_name(
    Person& person, const std::string& person_iri, librdf_world* world, librdf_model* model) {

    spdlog::debug("retrieve_person_name: Attempting to retrieve the preferred name");

    retrieve_result preferred_res =
        retrieve_person_preferred_name(person, person_iri, world, model);

    spdlog::debug(
        "retrieve_person_name: The result code of the retrieve_person_preferred_name function is:"
        " {}", preferred_res);

    assert(preferred_res != retrieve_result::Uninitialized);

    if (preferred_res == retrieve_result::QueryError) {
        spdlog::critical(
            "retrieve_person_name: Preferred name retrieval failed due to a query error");

        return retrieve_result::QueryError;
    } else if (preferred_res == retrieve_result::Success) {
        spdlog::debug("retrieve_person_name: Preferred name retrieved");

        return retrieve_result::Success;
    }

    spdlog::debug("retrieve_person_name: Attempting to retrieve the birth name");

    retrieve_result birth_res =
        retrieve_person_birth_name(person, person_iri, world, model);

    spdlog::debug(
        "retrieve_person_name: The result code of the retrieve_person_birth_name function is: {}",
        birth_res);

    assert(birth_res != retrieve_result::Uninitialized);

    if (birth_res == retrieve_result::QueryError) {
        spdlog::critical("retrieve_person_name: Birth name retrieval failed due to a query error");

        return retrieve_result::QueryError;
    } else if (birth_res == retrieve_result::Success) {
        spdlog::debug("retrieve_person_name: Birth name retrieved");

        return retrieve_result::Success;
    }

    spdlog::debug("retrieve_person_name: Attempting to retrieve any name");

    retrieve_result any_res =
        retrieve_person_any_name(person, person_iri, world, model);

    spdlog::debug(
        "retrieve_person_name: The result code of the retrieve_person_any_name function is: {}",
        any_res);

    assert(any_res != retrieve_result::Uninitialized);

    if (any_res == retrieve_result::QueryError) {
        spdlog::critical("retrieve_person_name: Any name retrieval failed due to a query error");

        return retrieve_result::QueryError;
    } else if (any_res == retrieve_result::Success) {
        spdlog::debug("retrieve_person_name: Some name retrieved");

        return retrieve_result::Success;
    }

    return retrieve_result::NotFound;
}


retrieve_result retrieve_person_any_name(
    Person& person, const std::string& person_iri, librdf_world* world, librdf_model* model) {

    const std::string query = R"(
        PREFIX gx: <http://gedcomx.org/>

        SELECT
            ?nameType, ?nameValue
        WHERE
        {
            {
                SELECT ?name
                WHERE
                {
                    ?person a gx:Person ;
                        gx:name ?name .
                    FILTER (?person = <)" + person_iri + R"(>)
                }
                LIMIT 1
            }
            ?name gx:nameForm ?form .
            ?form gx:part ?part .
            ?part gx:type ?nameType ;
                gx:value ?nameValue .
        })";

    spdlog::debug("retrieve_person_any_name: The query: {}", query);

    exec_query_result res = exec_query(world, model, query);

    if (!res->success) {
        spdlog::error(
            "retrieve_person_any_name: The query execution has failed");

        return retrieve_result::QueryError;
    }

    extract_data_table_result data_tuple = extract_data_table(res->results);
    const data_table& data_table = std::get<1>(data_tuple);

    if (data_table.empty()) {
        spdlog::debug(
            "retrieve_person_any_name: Properly formed names of person {} were not found",
            person_iri);

        return retrieve_result::NotFound;
    }

    extract_person_names(person, data_table);

    return retrieve_result::Success;
}


retrieve_result retrieve_person_birth_name(
    Person& person, const std::string& person_iri, librdf_world* world, librdf_model* model) {

    const std::string query = R"(
        PREFIX gx: <http://gedcomx.org/>

        SELECT
            ?nameType, ?nameValue
        WHERE
        {
            ?person a gx:Person ;
                gx:name ?name .
            ?name gx:type gx:BirthName ;
                gx:nameForm ?form .
            ?form gx:part ?part .
            ?part gx:type ?nameType ;
                gx:value ?nameValue .
            FILTER (?person = <)" + person_iri + R"(>)
        })";

    spdlog::debug("retrieve_person_birth_name: The query: {}", query);

    exec_query_result res = exec_query(world, model, query);

    if (!res->success) {
        spdlog::error("retrieve_person_birth_name: The query execution has failed");

        return retrieve_result::QueryError;
    }

    extract_data_table_result data_tuple = extract_data_table(res->results);
    const data_table& data_table = std::get<1>(data_tuple);

    if (data_table.empty()) {
        spdlog::debug(
            "retrieve_person_birth_name: Properly formed birth names of person {} were not found",
            person_iri);
        return retrieve_result::NotFound;
    }

    extract_person_names(person, data_table);

    return retrieve_result::Success;
}


retrieve_result retrieve_person_preferred_name(
    Person& person, const std::string& person_iri, librdf_world* world, librdf_model* model) {

    const std::string query = R"(
        PREFIX gx: <http://gedcomx.org/>
        PREFIX xsd: <http://www.w3.org/2001/XMLSchema#>

        SELECT
            ?nameType, ?nameValue
        WHERE
        {
            ?person a gx:Person ;
                gx:name ?name .
            ?name gx:preferred "true"^^xsd:boolean ;
                gx:nameForm ?form .
            ?form gx:part ?part .
            ?part gx:type ?nameType ;
                gx:value ?nameValue .
            FILTER (?person = <)" + person_iri + R"(>)
        })";

    spdlog::debug("retrieve_person_preferred_name: The query: {}", query);

    exec_query_result res = exec_query(world, model, query);

    if (!res->success) {
        spdlog::error("retrieve_person_preferred_name: The query execution has failed");

        return retrieve_result::QueryError;
    }

    extract_data_table_result data_tuple = extract_data_table(res->results);
    const data_table& data_table = std::get<1>(data_tuple);

    if (data_table.empty()) {
        spdlog::debug(
            "retrieve_person_preferred_name: Properly formed preferred names of person {} were not"
            " found", person_iri);
        return retrieve_result::NotFound;
    }

    extract_person_names(person, data_table);

    return retrieve_result::Success;
}


auto fmt::formatter<retrieve_result>::format(retrieve_result r, format_context& ctx) const
    -> format_context::iterator {
  string_view name = "<unknown>";
  switch (r) {
  case retrieve_result::Uninitialized: name = "<uninitialized>"; break;
  case retrieve_result::QueryError: name = "<query error>"; break;
  case retrieve_result::NotFound: name = "<not found>"; break;
  case retrieve_result::Success: name = "<success>"; break;
  }
  return formatter<string_view>::format(name, ctx);
}
