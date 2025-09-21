#include "person/queries/details.hpp"

#include <fmt/format.h>

#include "common/spdlog_utils.hpp"
#include "person/error.hpp"


namespace person
{

std::vector<std::shared_ptr<common::Person>> retrieve_person_partners(
    const common::Person* person, librdf_world* world, librdf_model* model)
{
    if (!person)
    {
        throw person_exception(
            person_exception::error_code::input_contract_error,
            fmt::format(
                "Precondition failure: person={} must satisfy !nullptr", fmt::ptr(person)));
    }

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
                    (?proband = <)" + person->get_uri_str() + R"(>))
        })";

    spdlog::debug("{}: The query: {}", __func__, query);

    common::exec_query_result res = common::exec_query(world, model, query);

    if (!res->success) {
        spdlog::error("{}: The query execution has failed", __func__);

        throw person_exception(
            person_exception::error_code::query_error,
            "Failed to execute the 'retrieve person partners' query");
    }

    const common::extract_data_table_result data_tuple = common::extract_data_table(res->results);
    const common::data_table& data_table = std::get<1>(data_tuple);

    if (data_table.empty()) {
        spdlog::debug(
            "{}: No partners of person {} were found", __func__, person->get_uri_str());

        return {};
    }

    std::vector<std::shared_ptr<common::Person>> partners;

    for (const common::data_row& row : data_table) {
        auto uri_it = common::get_binding_value_req(row, "partner");

        /* Exceptional path (resource not found): Propagate the exception
         * Normal path (resource found): Continue the execution */
        auto partner = retrieve_person_base_data_req(uri_it->second, world, model);

        retrieve_person_name(*partner, world, model);

        partners.push_back(partner);
    }

    return partners;
}

} // namespace person
