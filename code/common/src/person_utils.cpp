#include "common/person_utils.hpp"

#include "common/common_exception.hpp"

namespace common
{

bool ask_person_gender_unknown(
    const common::Resource* person, librdf_world* world, librdf_model* model)
{
    if (!person)
    {
        throw common_exception(
            common_exception::error_code::input_contract_error,
            fmt::format(
                "Precondition failure: person={} must satisfy !nullptr", fmt::ptr(person)));
    }

    return ask_person_gender_unknown(person->get_uri_str(), world, model);
}

bool ask_person_gender_unknown(
    const std::string_view person_uri, librdf_world* world, librdf_model* model)
{
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

    const char* query_id = "ask person gender unknown";
    constexpr std::string_view query_tmpl = R"(
        PREFIX gx: <http://gedcomx.org/>
        PREFIX rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#>

        ASK
        {{
            <{person}> gx:gender ?gender .
            ?gender rdf:type gx:Gender ;
                gx:type ?type .
        }}
    )";
    const std::string query = fmt::format(query_tmpl, fmt::arg("person", person_uri));

    spdlog::debug("{}: The '{}' query: {}", __func__, query_id, query);

    common::exec_query_result query_result = common::exec_query(world, model, query, query_id);

    const bool ask_result = common::extract_boolean_result(query_result->results);
    const bool return_value = !ask_result;

    spdlog::debug(
        "{}: The ask query result is '{}' (the function return value is '{}')",
        __func__, ask_result, return_value);

    return return_value;
}

} // namespace common
