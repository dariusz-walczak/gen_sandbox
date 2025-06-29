#include "person/command/common.hpp"

#include "common/file_system_utils.hpp"


input_files determine_input_paths(const cli_options& options)
{
    spdlog::trace("{}: Entry checkpoint", __func__);

    auto base_path = std::filesystem::path(options.base_path_raw);

    return merge_input_files(
        find_input_files(base_path, "ttl"),
        adapt_string_paths(options.input_paths));
}


scoped_redland_ctx load_input_data(const cli_options& options)
{
    spdlog::trace("{}: Entry checkpoint", __func__);

    input_files all_input_paths = determine_input_paths(options);

    scoped_redland_ctx redland_ctx = create_redland_ctx();
    initialize_redland_ctx(redland_ctx); // throws common_exception on initialization failure

    load_rdf_set(redland_ctx->world, redland_ctx->model, all_input_paths);

    return redland_ctx;
}
