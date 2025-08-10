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

using file_deps_lut = std::map<common::person_id, common::file_set>;
using person_id_set = std::set<common::person_id>;
using person_deps_lut = std::map<common::person_id, person_id_set>;


void collect_deps(librdf_world* world, librdf_model* model, file_deps_lut& file_deps)
{

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

    file_deps_lut file_lut;
    person_deps_lut person_lut;

    common::input_files input_paths = determine_input_paths(options);
    common::resource_set persons = retrieve_person_iris(options, input_paths);

    for (const auto& res : persons)
    {
        spdlog::info(res->get_unique_id());
    }

    for (const auto& path : input_paths)
    {
        common::scoped_redland_ctx redland_ctx = common::create_redland_ctx();
        initialize_redland_ctx(redland_ctx); // throws common_exception on initialization failure

        common::load_rdf(redland_ctx->world, redland_ctx->model, path.string());

        collect_deps(redland_ctx->world, redland_ctx->model, file_lut);
    }
}
