#include "queries.hpp"

#include "error.hpp"


retrieve_result retrieve_person(
    Person& person, const std::string& person_iri, librdf_world* world, librdf_model* model) {

    spdlog::trace("retrieve_person: Entry checkpoint");

    const std::string query = R"(
        PREFIX gx: <http://gedcomx.org/>

        SELECT ?person, ?genderType, ?birthDate, ?deathDate, ?father, ?fatherName,
            ?motherName
        WHERE {
            ?person a gx:Person .
            OPTIONAL {
                ?person gx:gender ?gender .
                ?gender a gx:Gender ;
                    gx:type ?genderType .
            }
            OPTIONAL {
                ?person gx:parent ?father .
                ?father a gx:Person ;
                    gx:name ?fatherName ;
                    gx:gender ?fatherGender .
                ?fatherGender a gx:Gender ;
                    gx:type gx:Male .
            }
            OPTIONAL {
                ?person gx:parent ?mother .
                ?mother a gx:Person ;
                    gx:name ?motherName ;
                    gx:gender ?motherGender .
                ?motherGender a gx:Gender ;
                    gx:type gx:Female .
            }
            ?person gx:birthDate ?birthDate .
            OPTIONAL {
                ?person gx:deathDate ?deathDate
            }
            FILTER (?person = <)" + person_iri + R"(>)
        })";

    spdlog::debug("retrieve_person: The query: {}", query);

    exec_query_result res = exec_query(world, model, query);

    if (!res->success) {
        spdlog::error("retrieve_person: The query execution has failed");

        throw person_exception(
            person_exception::error_code::query_error,
            "Failed to execute the 'retrieve person' query");
    }

    const extract_data_table_result data_tuple = extract_data_table(res->results);
    const data_table& data_table = std::get<1>(data_tuple);

    if (data_table.empty()) {
        spdlog::error("retrieve_person: Person {} was not found", person_iri);

        return retrieve_result::NotFound;
    }

    assert(data_table.size() == 1);

    const data_row& data_row = data_table.front();

    extract_person_gender(person, data_row, "genderType");

    return retrieve_result::Success;
}


retrieve_result retrieve_person_name(
    Person& person, const std::string& person_iri, librdf_world* world, librdf_model* model) {

    spdlog::debug("retrieve_person_name: Attempting to retrieve the preferred name");

    retrieve_result preferred_res =
        retrieve_person_preferred_name(person, person_iri, world, model);

    spdlog::debug(
        "retrieve_person_name: The result code of the retrieve_person_preferred_name function is:"
        " {}", preferred_res);

    if (preferred_res == retrieve_result::Success) {
        spdlog::debug("retrieve_person_name: Preferred name retrieved");

        return retrieve_result::Success;
    }

    spdlog::debug("retrieve_person_name: Attempting to retrieve the birth name");

    retrieve_result birth_res =
        retrieve_person_birth_name(person, person_iri, world, model);

    spdlog::debug(
        "retrieve_person_name: The result code of the retrieve_person_birth_name function is: {}",
        birth_res);

    if (birth_res == retrieve_result::Success) {
        spdlog::debug("retrieve_person_name: Birth name retrieved");

        return retrieve_result::Success;
    }

    spdlog::debug("retrieve_person_name: Attempting to retrieve any name");

    retrieve_result any_res =
        retrieve_person_any_name(person, person_iri, world, model);

    spdlog::debug(
        "retrieve_person_name: The result code of the retrieve_person_any_name function is: {}",
        any_res);

    if (any_res == retrieve_result::Success) {
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

        throw person_exception(
            person_exception::error_code::query_error,
            "Failed to execute the 'retrieve any name of a person' query");
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

        throw person_exception(
            person_exception::error_code::query_error,
            "Failed to execute the 'retrieve person birth name' query");
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

        throw person_exception(
            person_exception::error_code::query_error,
            "Failed to execute the 'retrieve person preferred name' query");
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


retrieve_result retrieve_person_relatives(
    Person& person, const std::string& person_iri, librdf_world* world, librdf_model* model) {

    return retrieve_result::NotFound;
}


retrieve_result retrieve_person_parents(
    Person& person, const std::string& person_iri, librdf_world* world, librdf_model* model) {

    const std::string query = R"(
        PREFIX gx: <http://gedcomx.org/>
        PREFIX xsd: <http://www.w3.org/2001/XMLSchema#>

        SELECT
            ?relPerson ?relGender
        WHERE
        {
            ?rel a gx:Relationship ;
                gx:person1 ?relPerson ;
                gx:person2 ?proband ;
                gx:type gx:ParentChild .
            ?relPerson a gx:Person ;
                gx:gender ?genderConclusion .
            ?genderConclusion gx:type ?relGender .
            FILTER (?proband = <)" + person_iri + R"(>)
        })";

    spdlog::debug("retrieve_person_parents: The query: {}", query);

    exec_query_result res = exec_query(world, model, query);

    if (!res->success) {
        spdlog::error("retrieve_person_parents: The query execution has failed");

        throw person_exception(
            person_exception::error_code::query_error,
            "Failed to execute the 'retrieve person parents' query");
    }

    const extract_data_table_result data_tuple = extract_data_table(res->results);
    const data_table& data_table = std::get<1>(data_tuple);

    if (data_table.empty()) {
        spdlog::debug(
            "retrieve_person_parents: No parents of person {} were found", person_iri);

        return retrieve_result::NotFound;
    }

    for (data_row row : data_table) {
        Person parent;

        extract_person_gender(parent, row, "relGender");

        retrieve_result name_res = retrieve_person_name(
            parent, row["relPerson"], world, model);

        if (parent.gender == Gender::Male) {
            if (person.father) {
                spdlog::error(
                    "retrieve_person_parents: Found multiple fathers of the person {}. Ignoring"
                    " the later occurrence", person_iri);
            } else {
                spdlog::debug("retrieve_person_parents: Father found");

                person.father = std::make_shared<Person>(parent);
            }
        } else if (parent.gender == Gender::Female) {
            if (person.mother) {
                spdlog::error(
                    "retrieve_person_parents: Found multiple mothers of the person {}. Ignoring"
                    " the later occurrence", person_iri);
            } else {
                spdlog::debug("retrieve_person_parents: Mother found");

                person.mother = std::make_shared<Person>(parent);
            }
        }
    }

    //extract_person_names(person, data_table);

    return retrieve_result::Success;
}


auto fmt::formatter<retrieve_result>::format(retrieve_result r, format_context& ctx) const
    -> format_context::iterator {
  string_view name = "<unknown>";
  switch (r) {
  case retrieve_result::NotFound: name = "<not found>"; break;
  case retrieve_result::Success: name = "<success>"; break;
  }
  return formatter<string_view>::format(name, ctx);
}
