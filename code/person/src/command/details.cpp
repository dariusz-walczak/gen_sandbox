#include "person/command/details.hpp"

#include <iostream>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "common/person.hpp"
#include "common/redland_utils.hpp"
#include "person/command/common.hpp"
#include "person/error.hpp"
#include "person/queries.hpp"



void run_details_command(const app_options& options)
{
    spdlog::trace("{}: Entry checkpoint", __func__);

    scoped_redland_ctx redland_ctx = load_input_data(options);

    const std::string person_iri = compose_person_iri(options.details_cmd.person_id);

    Person person;

    retrieve_result person_res =
        retrieve_person_base_data_opt(person, person_iri, redland_ctx->world, redland_ctx->model);

    if (person_res == retrieve_result::NotFound) {
        spdlog::debug("{}: Person '{}' not found", __func__, options.details_cmd.person_id);

        throw person_exception(
            person_exception::error_code::resource_not_found,
            fmt::format("Person not found: {}", options.details_cmd.person_id));
    }

    retrieve_person_name(person, person_iri, redland_ctx->world, redland_ctx->model);
    retrieve_person_parents(person, person_iri, redland_ctx->world, redland_ctx->model);
    retrieve_person_partners(person, person_iri, redland_ctx->world, redland_ctx->model);
    retrieve_person_children(person, person_iri, redland_ctx->world, redland_ctx->model);

    nlohmann::json output = person_to_json(person);
    std::cout << output.dump(4) << std::endl;
}
