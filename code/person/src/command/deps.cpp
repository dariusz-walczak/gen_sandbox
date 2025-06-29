#include "person/command/deps.hpp"

#include <iostream>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "common/person.hpp"
#include "common/redland_utils.hpp"
#include "person/command/common.hpp"
#include "person/queries/common.hpp"

void run_deps_command(const cli_options& options)
{
    spdlog::trace("{}: Entry checkpoint", __func__);

    scoped_redland_ctx redland_ctx = load_input_data(options);

    std::cout << "Dummy command\n";
}
