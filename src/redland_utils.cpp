#include "redland_utils.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <spdlog/spdlog.h>
#include <tabulate/tabulate.hpp>

namespace {
    struct load_rdf_ctx {
        librdf_parser* parser;
        librdf_uri*    base_uri;
    };

    void release_load_rdf_ctx(load_rdf_ctx* ctx) {
        librdf_free_uri(ctx->base_uri);
        spdlog::debug("Released the Base URI");

        librdf_free_parser(ctx->parser);
        spdlog::debug("Released the Redland Parser");

        delete ctx;
    }

    typedef
        std::unique_ptr<load_rdf_ctx, decltype(&release_load_rdf_ctx)>
        scoped_load_rdf_ctx;
}

void load_rdf(librdf_world* world, librdf_model* model, const std::string& input_file_path) {

    scoped_load_rdf_ctx ctx = { new load_rdf_ctx(), release_load_rdf_ctx };

    // https://librdf.org/docs/api/redland-parser.html#librdf-new-parser
    ctx->parser = librdf_new_parser(world, "turtle", nullptr, nullptr);

    if (!ctx->parser) {
        spdlog::error("load_rdf: Failed to create a Redland Parser");

        return;
    }

    spdlog::debug("load_rdf: Created a Redland Parser");

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


namespace {
    struct exec_query_ctx {
        librdf_query* query;
        librdf_query_results* results;
    };

    void release_exec_query_ctx(exec_query_ctx* ctx) {
        librdf_free_query_results(ctx->results);
        spdlog::debug("Released the Redland Query Results");

        librdf_free_query(ctx->query);
        spdlog::debug("Released the Redland Query");

        delete ctx;
    }

    typedef
        std::unique_ptr<exec_query_ctx, decltype(&release_exec_query_ctx)>
        exec_query_ctx_unique_ptr;


    struct binding_ctx {
        librdf_node*   node;
        unsigned char* value;
    };

    void release_binding_ctx(binding_ctx* ctx) {
        free(ctx->value);
        librdf_free_node(ctx->node);

        spdlog::debug("Released the result row context");
    }

    typedef
        std::unique_ptr<binding_ctx, decltype(&release_binding_ctx)>
        scoped_binding_ctx;
}

void exec_query(librdf_world* world, librdf_model* model, const std::string& query_text) {

    exec_query_ctx_unique_ptr ctx = { new exec_query_ctx(), release_exec_query_ctx };

    ctx->query = librdf_new_query(
        world, "sparql", nullptr, reinterpret_cast<const unsigned char*>(query_text.c_str()), nullptr);

    if (!ctx->query) {
        spdlog::error("exec_query: Failed to create a Redland Query");

        return;
    }

    spdlog::debug("exec_query: Created a Redland Query");

    ctx->results = librdf_query_execute(ctx->query, model);
    
    if (!ctx->results) {
        spdlog::error("exec_query: Redland Query execution failed");

        return;
    }

    spdlog::debug("exec_query: Redland Query execution succeeded");

    tabulate::Table table;

    // Add the header row:
    if (!librdf_query_results_finished(ctx->results)) {
        const int binding_count = librdf_query_results_get_bindings_count(ctx->results);

        tabulate::Table::Row_t row;

        for (int i=0; i<binding_count; ++i) {
            scoped_binding_ctx bind_ctx = { new binding_ctx(), release_binding_ctx };
            bind_ctx->node = librdf_query_results_get_binding_value(ctx->results, i);

            if (bind_ctx->node) {
                row.push_back(librdf_query_results_get_binding_name(ctx->results, i));
            } else {
                spdlog::warn("exec_query: Node #{} of the first row couldn't be retrieved", i);
            }
        }

        table.add_row(row);
    }

    while (!librdf_query_results_finished(ctx->results)) {
        int binding_count = librdf_query_results_get_bindings_count(ctx->results);

        tabulate::Table::Row_t row;

        for (int i=0; i<binding_count; ++i) {
            scoped_binding_ctx bind_ctx = { new binding_ctx(), release_binding_ctx };
            bind_ctx->node = librdf_query_results_get_binding_value(ctx->results, i);

            if (bind_ctx->node) {
                bind_ctx->value = librdf_node_to_string(bind_ctx->node);

                if (bind_ctx->value) {
                    if (librdf_node_is_literal(bind_ctx->node)) {
                        row.push_back(
                            std::string(
                                reinterpret_cast<char*>(
                                    librdf_node_get_literal_value(bind_ctx->node))));
                    } else {
                        row.push_back(std::string(reinterpret_cast<char*>(bind_ctx->value)));
                    }
                } else {
                    row.push_back("");
                }
            }
        }
        
        table.add_row(row);

        librdf_query_results_next(ctx->results);
    }

    table.format().multi_byte_characters(true);

    std::cout << table << std::endl;
}
