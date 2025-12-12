#include "person/command/details.hpp"

#include <iostream>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "common/person.hpp"
#include "common/redland_utils.hpp"
#include "person/command/common.hpp"
#include "person/error.hpp"
#include "person/queries/common.hpp"
#include "person/queries/details.hpp"

namespace person
{

void run_details_command(const cli_options& options)
{
    spdlog::trace("{}: Entry checkpoint", __func__);

    common::scoped_redland_ctx redland_ctx = load_input_data(options);

    const std::string person_uri { options.details_cmd.person_uri };

    // Exceptional path (resource not found): Propagate the exception
    std::shared_ptr<common::Person> person = 
        retrieve_person_base_data_req(person_uri, redland_ctx->world, redland_ctx->model);

    // Normal path (resource found): Continue the execution
    retrieve_person_name(*person, redland_ctx->world, redland_ctx->model);
    person->father = retrieve_person_father(
        person.get(), redland_ctx->world, redland_ctx->model, person->notes());
    person->mother = retrieve_person_mother(
        person.get(), redland_ctx->world, redland_ctx->model, person->notes());
    person->partners = retrieve_person_partners(
        person.get(), redland_ctx->world, redland_ctx->model, person->notes());

    retrieve_person_children(*person, redland_ctx->world, redland_ctx->model);

    nlohmann::json output = person_to_json(*person);
    std::cout << output.dump(4) << '\n';
}

} // namespace person
