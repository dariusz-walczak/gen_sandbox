#include "person/queries/common.hpp"

#include <fmt/format.h>

#include "person/error.hpp"


retrieve_result retrieve_person_base_data_opt(
    Person& person, const std::string& person_iri, librdf_world* world, librdf_model* model) {

    spdlog::trace(fmt::format("retrieve_person: Entry checkpoint ({})", person_iri));

    const std::string query = R"(
        PREFIX gx: <http://gedcomx.org/>

        SELECT ?person, ?genderType, ?birthDate, ?deathDate
        WHERE {
            ?person a gx:Person .
            OPTIONAL {
                ?person gx:gender ?gender .
                ?gender a gx:Gender ;
                    gx:type ?genderType .
            }
            OPTIONAL {
                ?person gx:birthDate ?birthDate
            }
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

    extract_person_id(person, data_row, "person");
    extract_person_gender(person, data_row, "genderType");
    extract_person_birth_date(person, data_row, "birthDate");
    extract_person_death_date(person, data_row, "deathDate");

    return retrieve_result::Success;
}

void retrieve_person_base_data_req(
    Person& person,
    const std::string& person_iri,
    librdf_world* world,
    librdf_model* model)
{
    retrieve_result result = retrieve_person_base_data_opt(person, person_iri, world, model);

    if (result == retrieve_result::NotFound)
    {
        throw person_exception(
            person_exception::error_code::resource_not_found,
            fmt::format("Person not found: '{}'", person_iri));
    }
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

        SELECT
            ?relPerson
        WHERE
        {
            ?rel a gx:Relationship ;
                gx:person1 ?relPerson ;
                gx:person2 ?proband ;
                gx:type gx:ParentChild .
            ?relPerson a gx:Person .
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

    for (const data_row& row : data_table) {
        auto iri_it = get_binding_value_req(row, "relPerson");

        std::shared_ptr<Person> parent = std::make_shared<Person>();
        retrieve_person_base_data_req(*parent, iri_it->second, world, model);
        retrieve_person_name(*parent, iri_it->second, world, model);

        if (parent->gender == Gender::Male) {
            if (person.father) {
                spdlog::error(
                    "retrieve_person_parents: Found multiple fathers of the person {}. Ignoring"
                    " the later occurrence", person_iri);
            } else {
                spdlog::debug("retrieve_person_parents: Father found");

                person.father = parent;
            }
        } else if (parent->gender == Gender::Female) {
            if (person.mother) {
                spdlog::error(
                    "retrieve_person_parents: Found multiple mothers of the person {}. Ignoring"
                    " the later occurrence", person_iri);
            } else {
                spdlog::debug("retrieve_person_parents: Mother found");

                person.mother = parent;
            }
        }
    }

    return retrieve_result::Success;
}


retrieve_result retrieve_person_partners(
    Person& person, const std::string& person_iri, librdf_world* world, librdf_model* model) {

    const std::string query = R"(
        PREFIX gx: <http://gedcomx.org/>

        SELECT DISTINCT ?partner
        WHERE
        {
            {
                ?rel a gx:Relationship ;
                    gx:person1 ?partner ;
                    gx:person2 ?proband ;
                    gx:type gx:Couple .
            }
            UNION
            {
                ?rel a gx:Relationship ;
                    gx:person1 ?proband ;
                    gx:person2 ?partner ;
                    gx:type gx:Couple .
            }
            UNION
            {
                ?rel1 a gx:Relationship ;
                    gx:person1 ?proband ;
                    gx:person2 ?child ;
                    gx:type gx:ParentChild .
                ?rel2 a gx:Relationship ;
                    gx:person1 ?partner ;
                    gx:person2 ?child ;
                    gx:type gx:ParentChild .
                ?child a gx:Person .
            }
            FILTER ((?partner != ?proband) &&
                    (?proband = <)" + person_iri + R"(>))
        })";

    spdlog::debug("retrieve_person_partners: The query: {}", query);

    exec_query_result res = exec_query(world, model, query);

    if (!res->success) {
        spdlog::error("retrieve_person_partners: The query execution has failed");

        throw person_exception(
            person_exception::error_code::query_error,
            "Failed to execute the 'retrieve person partners' query");
    }

    const extract_data_table_result data_tuple = extract_data_table(res->results);
    const data_table& data_table = std::get<1>(data_tuple);

    if (data_table.empty()) {
        spdlog::debug(
            "retrieve_person_partners: No partners of person {} were found", person_iri);

        return retrieve_result::NotFound;
    }

    for (const data_row& row : data_table) {
        auto iri_it = get_binding_value_req(row, "partner");

        std::shared_ptr<Person> partner = std::make_shared<Person>();

        retrieve_person_base_data_req(*partner, iri_it->second, world, model);
        retrieve_person_name(*partner, iri_it->second, world, model);

        person.partners.push_back(partner);
    }

    return retrieve_result::Success;
}


retrieve_result retrieve_person_children(
    Person& person, const std::string& person_iri, librdf_world* world, librdf_model* model) {

    const std::string query = R"(
        PREFIX gx: <http://gedcomx.org/>

        SELECT
            ?child ?partner
        WHERE
        {
            ?rel1 a gx:Relationship ;
                gx:person1 ?proband ;
                gx:person2 ?child ;
                gx:type gx:ParentChild .
            OPTIONAL {
                ?rel2 a gx:Relationship ;
                    gx:person1 ?partner ;
                    gx:person2 ?child ;
                    gx:type gx:ParentChild .
                FILTER(?partner != ?proband)
            }
            ?child a gx:Person .
            FILTER (?proband = <)" + person_iri + R"(>)
        })";

    spdlog::debug("retrieve_person_children: The query: {}", query);

    exec_query_result res = exec_query(world, model, query);

    if (!res->success) {
        spdlog::error("retrieve_person_children: The query execution has failed");

        throw person_exception(
            person_exception::error_code::query_error,
            "Failed to execute the 'retrieve person children' query");
    }

    const extract_data_table_result data_tuple = extract_data_table(res->results);
    const data_table& data_table = std::get<1>(data_tuple);

    if (data_table.empty()) {
        spdlog::debug(
            "retrieve_person_children: No children of person {} were found", person_iri);

        return retrieve_result::NotFound;
    }

    for (const data_row& row : data_table) {
        auto iri_it = get_binding_value_req(row, "child");

        std::shared_ptr<Person> child = std::make_shared<Person>();
        retrieve_person_base_data_req(*child, iri_it->second, world, model);
        retrieve_person_name(*child, iri_it->second, world, model);

        if (has_binding(row, "partner"))
        {
            Person partner;
            extract_person_id(partner, row, "partner");
            person.children[partner.id].push_back(child);
        }
        else
        {
            person.children[""].push_back(child);
        }
    }

    return retrieve_result::Success;
}


std::vector<Person> retrieve_person_list(librdf_world* world, librdf_model* model)
{
    spdlog::trace("retrieve_person_list: Entry checkpoint");

    const std::string query = R"(
        PREFIX gx: <http://gedcomx.org/>

        SELECT ?person, ?genderType, ?birthDate, ?deathDate
        WHERE {
            ?person a gx:Person .
            OPTIONAL {
                ?person gx:gender ?gender .
                ?gender a gx:Gender ;
                    gx:type ?genderType .
            }
            OPTIONAL {
                ?person gx:birthDate ?birthDate
            }
            OPTIONAL {
                ?person gx:deathDate ?deathDate
            }
        })";

    spdlog::debug("retrieve_person_list: The query: {}", query);

    exec_query_result res = exec_query(world, model, query);

    if (!res->success) {
        spdlog::error("retrieve_person_list: The query execution has failed");

        throw person_exception(
            person_exception::error_code::query_error,
            "Failed to execute the 'retrieve person_list' query");
    }

    const extract_data_table_result data_tuple = extract_data_table(res->results);
    const data_table& data_table = std::get<1>(data_tuple);

    std::vector<Person> result;
    result.reserve(data_table.size());

    for (const data_row& row : data_table)
    {
        result.emplace_back();

        extract_person_id(result.back(), row, "person");
        extract_person_gender(result.back(), row, "genderType");
        extract_person_birth_date(result.back(), row, "birthDate");
        extract_person_death_date(result.back(), row, "deathDate");
        retrieve_person_name(result.back(), result.back().iri, world, model);
    }

    return result;
}


auto fmt::formatter<retrieve_result>::format(retrieve_result res, format_context& ctx) const
    -> format_context::iterator
{
    string_view name = "<unknown>";

    switch (res)
    {
    case retrieve_result::NotFound:
        name = "<not found>";
        break;
    case retrieve_result::Success:
        name = "<success>";
        break;
    }

    return formatter<string_view>::format(name, ctx);
}
