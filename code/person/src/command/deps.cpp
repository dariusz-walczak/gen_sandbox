#include "person/command/deps.hpp"

#include <iostream>

#include <spdlog/spdlog.h>

#include "common/file_system_utils.hpp"
#include "common/person.hpp"
#include "common/redland_utils.hpp"
#include "person/command/common.hpp"
#include "person/queries/common.hpp"
#include "person/queries/deps.hpp"

namespace person
{
namespace detail
{

using file_deps_lut = std::map<common::Resource, common::file_set>;
using person_deps_lut = std::map<common::Resource, std::set<common::Resource>>;


void collect_dependent_resources(
    librdf_world* world, librdf_model* model,
    const common::resource_set& persons,
    const std::filesystem::path& data_file_path,
    file_deps_lut& data_file_deps)
{
    spdlog::debug(
        "{}: Collecting list of resources dependent on the '{}' data file",
        __func__, data_file_path);

    for (const auto& person : persons)
    {
        if (ask_resource_referenced(world, model, person))
        {
            data_file_deps[*person].insert(data_file_path);

            spdlog::debug(
                "{}: {} is referenced in {}",
                __func__, person->get_unique_id(), data_file_path);
        }
    }
}


person_deps_lut collect_dependent_persons(librdf_world* world, librdf_model* model)
{
    person_deps_lut deps;
    const common::data_table related_persons = retrieve_related_persons(world, model);
    //const common::data_table indirect_partners = ...

    for (const common::data_row& row : related_persons)
    {
        // The common::get_binding_value_req function throws the common::common_exception on a
        //  missing binding and always returns a valid binding, so it is safe to use the arrow
        //  operator on its result.
        const common::Resource p1 { common::get_binding_value_req(row, "person1")->second };
        const common::Resource p2 { common::get_binding_value_req(row, "person2")->second };

        deps[p1].insert(p2);
        deps[p2].insert(p1);
    }

    return deps;
}


file_deps_lut merge_dependencies(
    const person_deps_lut& person_deps, const file_deps_lut& file_deps)
{
    file_deps_lut merged_deps { file_deps };

    for (const auto& [dependent, dependencies] : person_deps)
    {
        for (const auto& dependency : dependencies)
        {
            const auto& src_it = file_deps.find(dependency);

            if (src_it != file_deps.end())
            {
                const common::file_set& src = src_it->second;
                merged_deps[dependent].insert(src.begin(), src.end());
            }
            else
            {
                spdlog::warn(
                    "{}: No file dependencies found for the '{}' person",
                    __func__, dependent.get_unique_id());
            }
        }
    }

    return merged_deps;
}

void print_dependencies(
    const file_deps_lut& final_file_lut, const std::optional<std::string>& src_path,
    const std::string& src_var_name, const std::string& int_var_name,
    const std::string& out_var_name)
{
    for (const auto& [person, deps] : final_file_lut)
    {
        const common::resource_id person_id = person.get_unique_id();
        const std::string out_file_path = fmt::format("$({})/{}.html", out_var_name, person_id);
        const std::string int_file_path = fmt::format("$({})/{}.json", int_var_name, person_id);

        std::cout << fmt::format("{}: {}\n\n", out_file_path, int_file_path);
        // The line-continuation character ('\') and the new-line character are added by the first
        //  dependency line printed in the following loop.
        std::cout << fmt::format("{}:", int_file_path);

        for (const auto& file : deps)
        {
            if (src_path.has_value() && file.string().starts_with(src_path.value()))
            {
                std::cout << fmt::format(
                    " \\\n\t\t$({})/{}",
                    src_var_name, file.string().substr(src_path.value().length()));
            }
            else
            {
                std::cout << fmt::format(" \\\n\t\t{}", file);
            }
        }

        // The leading new-line characters are needed to close the last of the dependency lines
        //  printed in the above loop.
        std::cout << fmt::format("\n\nout_person_all: {}\n", out_file_path);
        std::cout << fmt::format("int_person_all: {}\n\n", int_file_path);
    }
}

} // namespace detail

void run_deps_command(const cli_options& options)
{
    spdlog::trace("{}: Entry checkpoint", __func__);

    detail::file_deps_lut data_file_lut;
    detail::person_deps_lut person_deps;
    common::resource_set all_persons;

    common::input_files input_paths = determine_input_paths(options);

    {
        common::scoped_redland_ctx redland_ctx = common::create_redland_ctx();
        initialize_redland_ctx(redland_ctx); // throws common_exception on initialization failure
        common::load_rdf_set(redland_ctx->world, redland_ctx->model, input_paths);

        all_persons = retrieve_person_iris(redland_ctx->world, redland_ctx->model);
        person_deps = detail::collect_dependent_persons(redland_ctx->world, redland_ctx->model);
    }

    for (const auto& path : input_paths)
    {
        common::scoped_redland_ctx redland_ctx = common::create_redland_ctx();
        initialize_redland_ctx(redland_ctx); // throws common_exception on initialization failure
        common::load_rdf(redland_ctx->world, redland_ctx->model, path.string());

        detail::collect_dependent_resources(
            redland_ctx->world, redland_ctx->model,
            all_persons, path, data_file_lut);
    }

    detail::file_deps_lut final_file_lut = detail::merge_dependencies(person_deps, data_file_lut);

    detail::print_dependencies(
        final_file_lut, options.base_path_raw, options.deps_cmd.src_root_symbol,
        options.deps_cmd.int_root_symbol, options.deps_cmd.out_root_symbol);
}

} // namespace person
