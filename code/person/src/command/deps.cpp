#include "person/command/deps.hpp"

#include <iostream>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "common/file_system_utils.hpp"
#include "common/person.hpp"
#include "common/redland_utils.hpp"
#include "person/command/common.hpp"
#include "person/queries/common.hpp"
#include "person/queries/deps.hpp"

using file_deps_lut = std::map<common::Resource, common::file_set>;
using person_deps_lut = std::map<common::Resource, common::resource_set>;


void collect_dependent_resources(
    librdf_world* world, librdf_model* model,
    const common::resource_set& persons,
    const std::filesystem::path& data_file_path,
    file_deps_lut& data_file_deps)
{
    spdlog::debug("Collecting list of resources dependent on the '{}' data file", data_file_path);

    for (const auto& person : persons)
    {
        if (ask_resource_referenced(world, model, person))
        {
            data_file_deps[*person].insert(data_file_path);

            spdlog::debug("{} is referenced in {}", person->get_unique_id(), data_file_path);
        }
    }
}


common::resource_set retrieve_person_iris(
    const cli_options& options, const common::input_files& input_file_paths)
{
    common::scoped_redland_ctx redland_ctx = common::create_redland_ctx();
    initialize_redland_ctx(redland_ctx); // throws common_exception on initialization failure

    common::load_rdf_set(redland_ctx->world, redland_ctx->model, input_file_paths);

    return retrieve_person_iris(redland_ctx->world, redland_ctx->model);
}

void run_deps_command(const cli_options& options)
{
    spdlog::trace("{}: Entry checkpoint", __func__);

    file_deps_lut data_file_lut;
    person_deps_lut person_lut;

    common::input_files input_paths = determine_input_paths(options);
    common::resource_set persons = retrieve_person_iris(options, input_paths);

    for (const auto& path : input_paths)
    {
        common::scoped_redland_ctx redland_ctx = common::create_redland_ctx();
        initialize_redland_ctx(redland_ctx); // throws common_exception on initialization failure

        common::load_rdf(redland_ctx->world, redland_ctx->model, path.string());

        collect_dependent_resources(
            redland_ctx->world, redland_ctx->model,
            persons, path, data_file_lut);
    }
}
