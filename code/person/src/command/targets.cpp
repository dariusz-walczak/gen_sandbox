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
    const common::resource_set& resources, const std::filesystem::path& target_root_path,
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

        const std::filesystem::path tgt_path =
            (target_root_path / res->get_unique_id()).replace_extension(target_ext);

        std::cout << tgt_path.string();
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
        options.targets_cmd.tgt_root_path, (options.targets_cmd.json_flag ? "json" : "html"));
}

} // namespace person
