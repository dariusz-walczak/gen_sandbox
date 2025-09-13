#include "person/command/details.hpp"

#include <iostream>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "common/person.hpp"
#include "common/redland_utils.hpp"
#include "person/command/common.hpp"
#include "person/error.hpp"
#include "person/queries/common.hpp"

namespace person
{

void run_details_command(const cli_options& options)
{
    spdlog::trace("{}: Entry checkpoint", __func__);

    common::scoped_redland_ctx redland_ctx = load_input_data(options);

    const std::string person_uri { options.details_cmd.person_uri };

    common::Person person;

    retrieve_result person_res =
        retrieve_person_base_data_opt(person, person_uri, redland_ctx->world, redland_ctx->model);

    if (person_res == retrieve_result::NotFound) {
        spdlog::debug("{}: Person '{}' not found", __func__, person_uri);

        throw person_exception(
            person_exception::error_code::resource_not_found,
            fmt::format("Person not found: {}", person_uri));
    }

    retrieve_person_name(person, person_uri, redland_ctx->world, redland_ctx->model);
    retrieve_person_parents(person, person_uri, redland_ctx->world, redland_ctx->model);
    retrieve_person_partners(person, person_uri, redland_ctx->world, redland_ctx->model);
    retrieve_person_children(person, person_uri, redland_ctx->world, redland_ctx->model);

    nlohmann::json output = person_to_json(person);
    std::cout << output.dump(4) << '\n';
}

} // namespace person
