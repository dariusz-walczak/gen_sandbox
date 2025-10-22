#include "person/queries/common.hpp"

#include <fmt/format.h>

#include "person/error.hpp"

namespace person
{

std::shared_ptr<common::Person> retrieve_person_base_data_opt(
    const std::string& person_uri, librdf_world* world, librdf_model* model)
{

    spdlog::trace(fmt::format("retrieve_person: Entry checkpoint ({})", person_uri));

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
            FILTER (?person = <)" + person_uri + R"(>)
        })";

    spdlog::debug("retrieve_person: The query: {}", query);

    common::exec_query_result res = common::exec_query(world, model, query, __func__);
    const common::extract_data_table_result data_tuple = common::extract_data_table(res->results);
    const common::data_table& data_table = std::get<1>(data_tuple);

    if (data_table.empty()) {
        spdlog::debug("{}: Person not found: {}", __func__, person_uri);

        return { nullptr };
    }

    assert(data_table.size() == 1);

    const common::data_row& data_row = data_table.front();

    auto person = common::extract_resource<common::Person>(data_row, "person");

    extract_person_gender(*person, data_row, "genderType");
    extract_person_birth_date(*person, data_row, "birthDate");
    extract_person_death_date(*person, data_row, "deathDate");

    return person;
}

std::shared_ptr<common::Person> retrieve_person_base_data_req(
    const std::string& person_uri, librdf_world* world, librdf_model* model)
{
    auto person = retrieve_person_base_data_opt(person_uri, world, model);

    if (!person)
    {
        spdlog::error("{}: Person not found: {}", __func__, person_uri);

        throw person_exception(
            person_exception::error_code::resource_not_found,
            fmt::format("Person not found: '{}'", person_uri));
    }

    return person;
}


retrieve_result retrieve_person_name(
    common::Person& person, librdf_world* world, librdf_model* model)
{

    spdlog::debug("{}: Attempting to retrieve the preferred name", __func__);

    retrieve_result preferred_res =
        retrieve_person_preferred_name(person, world, model);

    spdlog::debug(
        "{}: The result code of the retrieve_person_preferred_name function is: {}",
        __func__, preferred_res);

    if (preferred_res == retrieve_result::Success) {
        spdlog::debug("{}: Preferred name retrieved", __func__);

        return retrieve_result::Success;
    }

    spdlog::debug("{}: Attempting to retrieve the birth name", __func__);

    retrieve_result birth_res =
        retrieve_person_birth_name(person, world, model);

    spdlog::debug(
        "{}: The result code of the retrieve_person_birth_name function is: {}",
        __func__, birth_res);

    if (birth_res == retrieve_result::Success) {
        spdlog::debug("{}: Birth name retrieved", __func__);

        return retrieve_result::Success;
    }

    spdlog::debug("{}: Attempting to retrieve any name", __func__);

    retrieve_result any_res =
        retrieve_person_any_name(person, world, model);

    spdlog::debug(
        "{}: The result code of the retrieve_person_any_name function is: {}", __func__, any_res);

    if (any_res == retrieve_result::Success) {
        spdlog::debug("{}: Some name retrieved", __func__);

        return retrieve_result::Success;
    }

    return retrieve_result::NotFound;
}


retrieve_result retrieve_person_any_name(
    common::Person& person, librdf_world* world, librdf_model* model)
{
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
                    FILTER (?person = <)" + person.get_uri_str() + R"(>)
                }
                LIMIT 1
            }
            ?name gx:nameForm ?form .
            ?form gx:part ?part .
            ?part gx:type ?nameType ;
                gx:value ?nameValue .
        })";

    spdlog::debug("retrieve_person_any_name: The query: {}", query);

    common::exec_query_result res = common::exec_query(world, model, query, __func__);
    const common::extract_data_table_result data_tuple = common::extract_data_table(res->results);
    const common::data_table& data_table = std::get<1>(data_tuple);

    if (data_table.empty()) {
        spdlog::debug(
            "retrieve_person_any_name: Properly formed names of person {} were not found",
            person.get_uri_str());

        return retrieve_result::NotFound;
    }

    extract_person_names(person, data_table);

    return retrieve_result::Success;
}


retrieve_result retrieve_person_birth_name(
    common::Person& person, librdf_world* world, librdf_model* model)
{
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
            FILTER (?person = <)" + person.get_uri_str() + R"(>)
        })";

    spdlog::debug("retrieve_person_birth_name: The query: {}", query);

    common::exec_query_result res = common::exec_query(world, model, query, __func__);
    const common::extract_data_table_result data_tuple = common::extract_data_table(res->results);
    const common::data_table& data_table = std::get<1>(data_tuple);

    if (data_table.empty()) {
        spdlog::debug(
            "retrieve_person_birth_name: Properly formed birth names of person {} were not found",
            person.get_uri_str());
        return retrieve_result::NotFound;
    }

    extract_person_names(person, data_table);

    return retrieve_result::Success;
}


common::resource_set retrieve_person_uris(librdf_world* world, librdf_model* model)
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

    common::exec_query_result res = common::exec_query(world, model, query, __func__);
    const common::extract_data_table_result data_tuple = common::extract_data_table(res->results);
    const common::data_table& data_table = std::get<1>(data_tuple);

    common::resource_set result;

    for (const common::data_row& row : data_table)
    {
        result.insert(common::extract_resource<common::Resource>(row, "person")); // throws common_exception
    }

    return result;
}


retrieve_result retrieve_person_preferred_name(
    common::Person& person, librdf_world* world, librdf_model* model)
{
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
            FILTER (?person = <)" + person.get_uri_str() + R"(>)
        })";

    spdlog::debug("retrieve_person_preferred_name: The query: {}", query);

    common::exec_query_result res = common::exec_query(world, model, query, __func__);
    const common::extract_data_table_result data_tuple = common::extract_data_table(res->results);
    const common::data_table& data_table = std::get<1>(data_tuple);

    if (data_table.empty()) {
        spdlog::debug(
            "retrieve_person_preferred_name: Properly formed preferred names of person {} were not"
            " found", person.get_uri_str());
        return retrieve_result::NotFound;
    }

    extract_person_names(person, data_table);

    return retrieve_result::Success;
}


retrieve_result retrieve_person_children(
    common::Person& person, librdf_world* world, librdf_model* model)
{
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
            FILTER (?proband = <)" + person.get_uri_str() + R"(>)
        })";

    spdlog::debug("{}: The query: {}", __func__, query);

    common::exec_query_result res = common::exec_query(world, model, query, __func__);
    const common::extract_data_table_result data_tuple = common::extract_data_table(res->results);
    const common::data_table& data_table = std::get<1>(data_tuple);

    if (data_table.empty()) {
        spdlog::debug(
            "{}: No children of person {} were found", __func__, person.get_uri_str());

        return retrieve_result::NotFound;
    }

    for (const common::data_row& row : data_table) {
        auto uri_it = common::get_binding_value_req(row, "child");

        /* Exceptional path (resource not found): Propagate the exception
         * Normal path (resource found): Continue the execution */
        auto child = retrieve_person_base_data_req(uri_it->second, world, model);

        retrieve_person_name(*child, world, model);

        if (common::has_binding(row, "partner"))
        {
            auto partner = common::extract_resource<common::Resource>(row, "partner");
            person.children[*partner].push_back(child);
        }
        else
        {
            person.children[{}].push_back(child);
        }
    }

    return retrieve_result::Success;
}


std::vector<std::shared_ptr<common::Person>> retrieve_person_list(
    librdf_world* world, librdf_model* model)
{
    spdlog::trace("{}: Entry checkpoint", __func__);

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

    spdlog::debug("{}: The query: {}", __func__, query);

    common::exec_query_result res = common::exec_query(world, model, query, __func__);
    const common::extract_data_table_result data_tuple = common::extract_data_table(res->results);
    const common::data_table& data_table = std::get<1>(data_tuple);

    std::vector<std::shared_ptr<common::Person>> result;
    result.reserve(data_table.size());

    for (const common::data_row& row : data_table)
    {
        auto person = common::extract_resource<common::Person>(row, "person");

        extract_person_gender(*person, row, "genderType");
        extract_person_birth_date(*person, row, "birthDate");
        extract_person_death_date(*person, row, "deathDate");
        retrieve_person_name(*person, world, model);

        result.emplace_back(person);
    }

    return result;
}

} // namespace person

auto fmt::formatter<person::retrieve_result>::format(
    person::retrieve_result res, format_context& ctx) const -> format_context::iterator
{
    string_view name = "<unknown>";

    switch (res)
    {
    case person::retrieve_result::NotFound:
        name = "<not found>";
        break;
    case person::retrieve_result::Success:
        name = "<success>";
        break;
    }

    return formatter<string_view>::format(name, ctx);
}
