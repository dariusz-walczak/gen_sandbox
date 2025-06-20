#include "common/redland_utils.hpp"

#include <cassert>
#include <iostream>
#include <unordered_set>
#include <memory>
#include <optional>
#include <map>
#include <string>
#include <tuple>
#include <vector>

#include <spdlog/spdlog.h>
#include <tabulate/tabulate.hpp>

#include "common/common_exception.hpp"


void release_redland_ctx(redland_context* ctx) {
    librdf_free_model(ctx->model);
    spdlog::debug("{}: Released the redland model", __func__);

    librdf_free_storage(ctx->storage);
    spdlog::debug("{}: Released the redland storage", __func__);

    librdf_free_world(ctx->world);
    spdlog::debug("{}: Released the redland world", __func__);

    spdlog::info("{}: Released the redland context", __func__);

    delete ctx;
}

scoped_redland_ctx create_redland_ctx() {
    return { new redland_context(), release_redland_ctx };
}

void initialize_redland_ctx(scoped_redland_ctx& ctx) {
    ctx->world = librdf_new_world();

    if (!ctx->world) {
        spdlog::error("{}: Failed to create a new redland world", __func__);

        throw new common_exception(
            common_exception::error_code::redland_initialization_failed,
            "Failed to create a new redland world");
    }

    spdlog::debug("{}: Created a redland world", __func__);

    librdf_world_open(ctx->world);

    spdlog::debug("{}: Initialized the redland world", __func__);

    // https://librdf.org/docs/api/redland-storage.html#librdf-new-storage
    ctx->storage = librdf_new_storage(ctx->world, "memory", nullptr, nullptr);

    if (!ctx->storage) {
        spdlog::error("{}: Failed to create a new redland storage", __func__);

        throw common_exception(
            common_exception::error_code::redland_initialization_failed,
            "Failed to create a new redland storage");
    }

    spdlog::debug("{}: Created a new redland storage", __func__);

    // https://librdf.org/docs/api/redland-model.html#librdf-new-model
    ctx->model = librdf_new_model(ctx->world, ctx->storage, nullptr);

    if (!ctx->model) {
        spdlog::error("{}: Failed to create a new redland model", __func__);

        throw new common_exception(
            common_exception::error_code::redland_initialization_failed,
            "Failed to create a new redland model");
    }

    spdlog::debug("{}: Created a new redland model", __func__);
}


namespace {
    struct load_rdf_ctx {
        librdf_parser* parser;
        librdf_uri*    base_uri;
    };

    void release_load_rdf_ctx(load_rdf_ctx* ctx) {
        librdf_free_uri(ctx->base_uri);
        spdlog::debug("Released the Base URI");

        librdf_free_parser(ctx->parser);
        spdlog::debug("Released the redland parser");

        delete ctx;
    }

    using scoped_load_rdf_ctx = std::unique_ptr<load_rdf_ctx, decltype(&release_load_rdf_ctx)>;
}

void load_rdf(librdf_world* world, librdf_model* model, const std::string& input_file_path) {

    scoped_load_rdf_ctx ctx = { new load_rdf_ctx(), release_load_rdf_ctx };

    // https://librdf.org/docs/api/redland-parser.html#librdf-new-parser
    ctx->parser = librdf_new_parser(world, "turtle", nullptr, nullptr);

    if (!ctx->parser) {
        spdlog::error("load_rdf: Failed to create a redland parser");

        return;
    }

    spdlog::debug("load_rdf: Created a redland parser");

    const unsigned char base_uri_str[] = "https://aurochsoft.com/";
    ctx->base_uri = librdf_new_uri(world, base_uri_str);

    if (!ctx->base_uri) {
        spdlog::critical("load_rdf: Failed to create the Base URI");

        return;
    }

    spdlog::debug("load_rdf: Created the Base URI");

    FILE* input_file = fopen(input_file_path.c_str(), "r");

    if (!input_file) {
        spdlog::error("road_rdf: Failed to open the '{}' file", input_file_path);

        return;
    }

    const int parser_error = librdf_parser_parse_file_handle_into_model(
        ctx->parser, input_file, 0, ctx->base_uri, model);

    fclose(input_file);

    spdlog::debug("road_rdf: Closed the '{}' file", input_file_path);

    if (parser_error) {
        spdlog::error("load_rdf: Failed to parse the '{}' input file", input_file_path);

        return;
    }

    spdlog::info("load_rdf: Successfully parsed the '{}' input file", input_file_path);
}


void release_exec_query_ctx(exec_query_ctx* ctx) {
    librdf_free_query_results(ctx->results);
    spdlog::debug("Released the redland query results");

    librdf_free_query(ctx->query);
    spdlog::debug("Released the redland query");

    delete ctx;
}

exec_query_result exec_query(librdf_world* world, librdf_model* model, const std::string& query) {

    spdlog::trace("exec_query: Entrypoint");

    exec_query_result res = { new exec_query_ctx(), release_exec_query_ctx };

    res->query = librdf_new_query(
        world, "sparql", nullptr, reinterpret_cast<const unsigned char*>(query.c_str()), nullptr);

    if (!res->query) {
        spdlog::error("exec_query: Failed to create a redland query");

        return res;
    }

    spdlog::debug("exec_query: Created a redland query");

    res->results = librdf_query_execute(res->query, model);

    if (!res->results) {
        spdlog::error("exec_query: Redland query execution failed");

        return res;
    }

    spdlog::debug("exec_query: Redland query execution succeeded");

    res->success = true;
    return res;
}

namespace {
    struct binding_ctx {
        librdf_node*   node;
        unsigned char* value;
    };

    void release_binding_ctx(binding_ctx* ctx) {
        free(ctx->value);
        librdf_free_node(ctx->node);

        spdlog::debug("Released the result row context");
    }

    using scoped_binding_ctx = std::unique_ptr<binding_ctx, decltype(&release_binding_ctx)>;
    using tmp_name_lut = std::unordered_set<binding_name>;
}


extract_data_table_result extract_data_table(librdf_query_results* results) {
    return extract_data_table(results, extract_cb_lut{});
}


extract_data_table_result extract_data_table(
    librdf_query_results* results, const extract_cb_lut& cb_lut)
{
    spdlog::trace("extract_data_table: Entrypoint");

    assert(results);

    head_row head_row;
    tmp_name_lut head_row_lut;
    data_table table;
    const int binding_count = librdf_query_results_get_bindings_count(results);

    if (binding_count < 0) {
        spdlog::error(
            "extract_data_table: Couldn't retrieve the number of bound variables. The "
            "librdf_query_results_get_bindings_count returned a negative number ({})",
            binding_count);

        return std::make_tuple(head_row, table);
    }

    int row_idx = 0;

    while (!librdf_query_results_finished(results)) {
        assert(
            (binding_count == librdf_query_results_get_bindings_count(results)) &&
            "Assuming that the binding count returned by librdf_query_results_get_bindings_count"
            " is the same for all the result rows");

        data_row row;

        for (int binding_idx=0; binding_idx < binding_count; ++binding_idx) {
            const char* binding_name = librdf_query_results_get_binding_name(results, binding_idx);

            if (head_row_lut.size() < static_cast<tmp_name_lut::size_type>(binding_count)) {
                if (!head_row_lut.count(binding_name)) {
                    head_row.emplace_back(binding_name);
                    head_row_lut.insert(binding_name);
                }
            }

            scoped_binding_ctx ctx = { new binding_ctx(), release_binding_ctx };
            ctx->node = librdf_query_results_get_binding_value(results, binding_idx);

            if (ctx->node) {
                std::string value;

                auto ecb_it = cb_lut.find(binding_name);

                if (ecb_it != cb_lut.end()) {
                    value = ecb_it->second(ctx->node);
                } else if (librdf_node_is_literal(ctx->node)) {
                    value = reinterpret_cast<char*>(
                        librdf_node_get_literal_value(ctx->node));
                } else if (librdf_node_is_resource(ctx->node)) {
                    librdf_uri* uri = librdf_node_get_uri(ctx->node);
                    std::string_view raw_value(
                        reinterpret_cast<char*>(
                            librdf_uri_as_string(uri)));

                    if (raw_value == "http://gedcomx.org/Male") {
                        value = "male";
                    } else if (raw_value == "http://gedcomx.org/Female") {
                        value = "female";
                    } else {
                        value = raw_value;
                    }
                } else {
                    ctx->value = librdf_node_to_string(ctx->node);

                    if (ctx->value) {
                        value = reinterpret_cast<char*>(ctx->value);
                    } else {
                        spdlog::debug(
                            "exec_query: Value of the node #{} of the row #{} couldn't be"
                            " retrieved", binding_idx, row_idx);
                    }
                }

                row.insert({std::string(binding_name), std::move(value)});
            } else {
                spdlog::debug(
                    "exec_query: Node #{} of the row #{} couldn't be retrieved",
                    binding_idx, row_idx);
            }
        }

        table.push_back(std::move(row));

        librdf_query_results_next(results);
        ++row_idx;
    }

    return std::make_tuple(head_row, table);
}

void print_data_table(const extract_data_table_result& data_table) {

    spdlog::trace("print_data_table: Entrypoint");

    auto [in_head_row, in_data_rows] = data_table;

    tabulate::Table table;

    {
        tabulate::Table::Row_t head_row;

        for (binding_name name : in_head_row) {
            head_row.emplace_back(name);
        }

        table.add_row(head_row);
    }

    for (data_row in_data_row : in_data_rows) {
        tabulate::Table::Row_t data_row;

        for (const binding_name& name : in_head_row) {
            auto in_data_row_it = in_data_row.find(name);

            if (in_data_row_it != in_data_row.end()) {
                data_row.emplace_back(in_data_row_it->second);
            } else {
                data_row.emplace_back("");
            }
        }

        table.add_row(data_row);
    }

    table.format().multi_byte_characters(true);

    std::cout << table << std::endl;
}


data_row::const_iterator get_binding_value_req(
    const data_row& row, const std::string& binding_name)
{
    auto binding_it = row.find(binding_name);

    if (binding_it != row.end())
    {
        return binding_it;
    }

    throw common_exception(
        common_exception::error_code::binding_not_found,
        fmt::format("The '{}' binding not found in the data row", binding_name));
}

bool has_binding(const data_row& row, const std::string& binding_name)
{
    return (row.find(binding_name) != row.end());
}
