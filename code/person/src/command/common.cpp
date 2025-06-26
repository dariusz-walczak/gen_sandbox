#include "person/command/common.hpp"

#include "common/file_system_utils.hpp"


scoped_redland_ctx load_input_data(const cli_options& options)
{
    spdlog::trace("{}: Entry checkpoint", __func__);

    auto base_path = std::filesystem::path(options.base_path_raw);

    auto all_input_paths = merge_input_files(
        find_input_files(base_path, "ttl"),
        adapt_string_paths(options.input_paths));

    scoped_redland_ctx redland_ctx = create_redland_ctx();

    initialize_redland_ctx(redland_ctx); // throws common_exception on initialization failure

    for (const auto& input_path : all_input_paths) {
        load_rdf(redland_ctx->world, redland_ctx->model, input_path.string());
    }

    return redland_ctx;
}
