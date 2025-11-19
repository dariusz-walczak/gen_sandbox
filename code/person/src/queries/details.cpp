#include "person/queries/details.hpp"

#include <fmt/format.h>

#include "common/spdlog_utils.hpp"
#include "person/error.hpp"


namespace person
{

namespace
{

common::Note create_inferred_partner_note(const std::shared_ptr<common::Person>& partner)
{
    return common::Note(
        common::Note::Type::Info, std::string(k_inferred_partner_note_id),
        {common::Variable{"partner", partner}},
        fmt::format("Partner inferred: {}", partner->get_uri_str()));
}

} // anonymous namespace

std::shared_ptr<common::Person> retrieve_person_father_opt(
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
        spdlog::debug(
            "{}: Father of proband {} wasn't found", __func__, proband->get_uri_str());

        return {};
    }
    else if (data_table.size() > 1)
    {
        throw person_exception(
            person_exception::error_code::multiple_resources_found,
            fmt::format("Too Many Resources: found {} fathers of the proband: {}",
                        data_table.size(), proband->get_uri_str()));
    }

    const auto& row = data_table.front();
    const auto& uri_it = common::get_binding_value_req(row, "father");
    auto parent = retrieve_person_base_data_req(uri_it->second, world, model);
    retrieve_person_name(*parent, world, model);

    return parent;
}

std::shared_ptr<common::Person> retrieve_person_mother_opt(
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
        spdlog::debug(
            "{}: Mother of proband {} wasn't found", __func__, proband->get_uri_str());

        return {};
    }
    else if (data_table.size() > 1)
    {
        throw person_exception(
            person_exception::error_code::multiple_resources_found,
            fmt::format("Too Many Resources: found {} mothers of the proband: {}",
                        data_table.size(), proband->get_uri_str()));
    }

    const auto& row = data_table.front();
    const auto& uri_it = common::get_binding_value_req(row, "mother");
    auto parent = retrieve_person_base_data_req(uri_it->second, world, model);
    retrieve_person_name(*parent, world, model);

    return parent;
}

std::vector<common::Person::PartnerRelation> retrieve_person_partners(
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

        SELECT DISTINCT ?partner, (MIN(?inferred_int) AS ?inferred)
        WHERE
        {
            {
                {
                    SELECT DISTINCT ?partner
                    WHERE
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

                        FILTER ((?partner != ?proband) &&
                                (?proband = <)" + proband->get_uri_str() + R"(>))
                    }
                }
                BIND(true AS ?inferred_int)
            }
            UNION
            {
                {
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
                        FILTER ((?partner != ?proband) &&
                                (?proband = <)" + proband->get_uri_str() + R"(>))
                    }
                }
                BIND(false AS ?inferred_int)
            }
        }
        GROUP BY ?partner)";

    spdlog::debug("{}: The query: {}", __func__, query);

    common::exec_query_result res = common::exec_query(world, model, query, __func__);
    const common::extract_data_table_result data_tuple = common::extract_data_table(res->results);
    const common::data_table& data_table = std::get<1>(data_tuple);

    if (data_table.empty())
    {
        spdlog::debug(
            "{}: No partners of proband {} were found", __func__, proband->get_uri_str());

        return {};
    }

    std::vector<common::Person::PartnerRelation> partners;

    for (const common::data_row& row : data_table)
    {
        if (row.empty())
        {
            continue;
        }

        const auto& uri_it = common::get_binding_value_req(row, "partner");
        const auto& inferred_it = common::get_binding_value_req(row, "inferred");
        const bool inferred = (inferred_it->second == "true");

        /* Exceptional path (resource not found): Propagate the exception
         * Normal path (resource found): Continue the execution */
        auto partner = retrieve_person_base_data_req(uri_it->second, world, model);

        retrieve_person_name(*partner, world, model);

        if (inferred)
        {
            notes.emplace_back(create_inferred_partner_note(partner));
        }

        partners.push_back({partner, inferred});
    }

    return partners;
}

} // namespace person
