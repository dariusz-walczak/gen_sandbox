#include "person/command/targets.hpp"

#include <iostream>

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "person/command/common.hpp"
#include "person/queries/common.hpp"

namespace person
{
namespace detail
{

void print_targets(
    const common::resource_set& resources, const std::string_view& target_root_symbol,
    const std::string_view& target_ext)
{
    for (bool first=true; const auto& res : resources)
    {
        if (!first)
        {
            std::cout << " ";
        }
        else
        {
            first = false;
        }

        std::cout << fmt::format(
            "$({})/{}.{}", target_root_symbol, res->get_unique_id(), target_ext);
    }

    std::cout << '\n';
}

} // namespace detail

void run_targets_command(const cli_options& options)
{
    spdlog::trace("{}: Entry checkpoint", __func__);

    common::input_files input_paths = determine_input_paths(options);

    common::scoped_redland_ctx redland_ctx = common::create_redland_ctx();
    initialize_redland_ctx(redland_ctx); // throws common_exception on initialization failure
    common::load_rdf_set(redland_ctx->world, redland_ctx->model, input_paths);

    detail::print_targets(
        retrieve_person_iris(redland_ctx->world, redland_ctx->model),
        options.targets_cmd.tgt_root_symbol, (options.targets_cmd.json_flag ? "json" : "html"));
}

} // namespace person
