#include "person/queries/details.hpp"

#include "common/data_table.hpp"
#include "common/string.hpp"
#include "common/variable_utils.hpp"
#include "person/error.hpp"


namespace person
{

namespace
{

common::Note create_multiple_fathers_note(
    const std::vector<std::shared_ptr<common::Person>>& fathers)
{
    return common::Note(
        common::Note::Type::Error, std::string(k_multiple_fathers_note_id),
        {construct_sequence_variable("fathers", fathers)},
        fmt::format(
            "Multiple fathers found:\n    {}",
            common::join(common::extract_uri_str_seq(fathers), "\n    ")));
}

common::Note create_multiple_mothers_note(
    const std::vector<std::shared_ptr<common::Person>>& mothers)
{
    return common::Note(
        common::Note::Type::Error, std::string(k_multiple_mothers_note_id),
        {construct_sequence_variable("mothers", mothers)},
        fmt::format(
            "Multiple mothers found:\n    {}",
            common::join(common::extract_uri_str_seq(mothers), "\n    ")));
}

} // anonymous namespace

std::shared_ptr<common::Person> retrieve_person_father(
    const common::Person* proband, librdf_world* world, librdf_model* model,
    std::vector<common::Note>& notes)
{
    if (!proband)
    {
        throw person_exception(
            person_exception::error_code::input_contract_error,
            fmt::format(
                "Precondition failure: proband={} must satisfy !nullptr", fmt::ptr(proband)));
    }

    if (!world)
    {
        throw person_exception(
            person_exception::error_code::input_contract_error,
            fmt::format(
                "Precondition failure: world={} must satisfy !nullptr", fmt::ptr(world)));
    }

    if (!model)
    {
        throw person_exception(
            person_exception::error_code::input_contract_error,
            fmt::format(
                "Precondition failure: model={} must satisfy !nullptr", fmt::ptr(model)));
    }

    const std::string query = R"(
        PREFIX gx: <http://gedcomx.org/>

        SELECT
            ?father
        WHERE
        {
            ?rel a gx:Relationship ;
                gx:person1 ?father ;
                gx:person2 ?proband ;
                gx:type gx:ParentChild .
            ?father a gx:Person ;
                gx:gender ?gender .
            ?gender a gx:Gender ;
                gx:type gx:Male .
            FILTER (?proband = <)" + proband->get_uri_str() + R"(>)
        })";

    const char* query_id = "retrieve proband father";

    spdlog::debug("{}: The '{}' query: {}", __func__, query_id, query);

    common::exec_query_result res = common::exec_query(world, model, query, __func__);
    const common::extract_data_table_result data_tuple = common::extract_data_table(res->results);
    const common::data_table& data_table = std::get<1>(data_tuple);

    if (data_table.empty())
    {
        spdlog::debug("{}: Father of proband {} wasn't found", __func__, proband->get_uri_str());
        return {};
    }
    else if (data_table.size() > 1)
    {
        spdlog::debug(
            "{}: Multiple ({}) fathers of proband {} were found",
            __func__, data_table.size(), proband->get_uri_str());

        notes.emplace_back(
            create_multiple_fathers_note(
                retrieve_person_caption_data_seq_req(
                    common::extract_resource_uri_seq(data_table, "father"),
                    world, model)));

        return {};
    }

    const auto& row = data_table.front();
    const auto& uri_it = common::get_binding_value_req(row, "father");
    auto parent = retrieve_person_base_data_req(uri_it->second, world, model);
    retrieve_person_name(*parent, world, model);

    return parent;
}

std::shared_ptr<common::Person> retrieve_person_mother(
    const common::Person* proband, librdf_world* world, librdf_model* model,
    std::vector<common::Note>& notes)
{
    if (!proband)
    {
        throw person_exception(
            person_exception::error_code::input_contract_error,
            fmt::format(
                "Precondition failure: proband={} must satisfy !nullptr", fmt::ptr(proband)));
    }

    if (!world)
    {
        throw person_exception(
            person_exception::error_code::input_contract_error,
            fmt::format(
                "Precondition failure: world={} must satisfy !nullptr", fmt::ptr(world)));
    }

    if (!model)
    {
        throw person_exception(
            person_exception::error_code::input_contract_error,
            fmt::format(
                "Precondition failure: model={} must satisfy !nullptr", fmt::ptr(model)));
    }

    const std::string query = R"(
        PREFIX gx: <http://gedcomx.org/>

        SELECT
            ?mother
        WHERE
        {
            ?rel a gx:Relationship ;
                gx:person1 ?mother ;
                gx:person2 ?proband ;
                gx:type gx:ParentChild .
            ?mother a gx:Person ;
                gx:gender ?gender .
            ?gender a gx:Gender ;
                gx:type gx:Female .
            FILTER (?proband = <)" + proband->get_uri_str() + R"(>)
        })";

    const char* query_id = "retrieve proband mother";

    spdlog::debug("{}: The '{}' query: {}", __func__, query_id, query);

    common::exec_query_result res = common::exec_query(world, model, query, __func__);
    const common::extract_data_table_result data_tuple = common::extract_data_table(res->results);
    const common::data_table& data_table = std::get<1>(data_tuple);

    if (data_table.empty())
    {
        spdlog::debug("{}: Mother of proband {} wasn't found", __func__, proband->get_uri_str());

        return {};
    }
    else if (data_table.size() > 1)
    {
        spdlog::debug(
            "{}: Multiple ({}) mothers of proband {} were found",
            __func__, data_table.size(), proband->get_uri_str());

        notes.emplace_back(
            create_multiple_mothers_note(
                retrieve_person_caption_data_seq_req(
                    common::extract_resource_uri_seq(data_table, "mother"),
                    world, model)));

        return {};
    }

    const auto& row = data_table.front();
    const auto& uri_it = common::get_binding_value_req(row, "mother");
    auto parent = retrieve_person_base_data_req(uri_it->second, world, model);
    retrieve_person_name(*parent, world, model);

    return parent;
}


std::vector<std::shared_ptr<common::Resource>> retrieve_person_parents(
    const common::Person* proband, librdf_world* world, librdf_model* model)
{
    if (!proband)
    {
        throw person_exception(
            person_exception::error_code::input_contract_error,
            fmt::format(
                "Precondition failure: proband={} must satisfy !nullptr", fmt::ptr(proband)));
    }

    if (!world)
    {
        throw person_exception(
            person_exception::error_code::input_contract_error,
            fmt::format(
                "Precondition failure: world={} must satisfy !nullptr", fmt::ptr(world)));
    }

    if (!model)
    {
        throw person_exception(
            person_exception::error_code::input_contract_error,
            fmt::format(
                "Precondition failure: model={} must satisfy !nullptr", fmt::ptr(model)));
    }

    const char* query_id = "retrieve person parents";
    constexpr std::string_view query_tmpl = R"(
        PREFIX gx: <http://gedcomx.org/>

        SELECT DISTINCT ?parent
        WHERE
        {{
            ?rel a gx:Relationship ;
                gx:person1 ?parent ;
                gx:person2 ?proband ;
                gx:type gx:ParentChild .

            FILTER (?proband = <{proband}>)
        }})";

    const std::string query = fmt::format(query_tmpl, fmt::arg("proband", proband->get_uri_str()));

    spdlog::debug("{}: The '{}' query: {}", __func__, query_id, query);

    common::exec_query_result res = common::exec_query(world, model, query, __func__);
    const common::extract_data_table_result data_tuple = common::extract_data_table(res->results);
    const common::data_table& data_table = std::get<1>(data_tuple);

    if (data_table.empty())
    {
        spdlog::debug(
            "{}: No parents of proband {} were found", __func__, proband->get_uri_str());

        return {};
    }

    std::vector<std::shared_ptr<common::Resource>> persons;
    persons.reserve(data_table.size());

    for (const common::data_row& row : data_table)
    {
        persons.emplace_back(common::extract_resource<common::Resource>(row, "parent"));
    }

    return persons;
}

} // namespace person
