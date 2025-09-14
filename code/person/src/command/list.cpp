#include "person/command/list.hpp"

#include <iostream>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "common/person.hpp"
#include "common/redland_utils.hpp"
#include "person/command/common.hpp"
#include "person/queries/common.hpp"

namespace person
{

void run_list_command(const cli_options& options)
{
    spdlog::trace("{}: Entry checkpoint", __func__);

    common::scoped_redland_ctx redland_ctx = load_input_data(options);

    auto persons = retrieve_person_list(redland_ctx->world, redland_ctx->model);

    nlohmann::json output = person_list_to_json(persons);
        std::cout << output.dump(4) << '\n';
}

} // namespace person
