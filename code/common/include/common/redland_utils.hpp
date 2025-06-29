#if !defined COMMON_REDLAND_UTILS_HPP
#define COMMON_REDLAND_UTILS_HPP

#include <map>
#include <string>

#include <redland.h>
#include <spdlog/spdlog.h>

#include "common/file_system_utils.hpp"


struct redland_context {
    librdf_world*   world;
    librdf_storage* storage;
    librdf_model*   model;
};

void release_redland_ctx(redland_context* ctx);

using scoped_redland_ctx = std::unique_ptr<redland_context, decltype(&release_redland_ctx)>;

scoped_redland_ctx create_redland_ctx();
/** Initialize a new Redland RDF Library context
 *
 * @throws common_exception when the context initialization fails. All Redland resources allocated
 *     before the failure are released automatically. */
void initialize_redland_ctx(scoped_redland_ctx& ctx);


void load_rdf(librdf_world* world, librdf_model* model, const std::string& input_file_path);
void load_rdf_set(librdf_world* world, librdf_model* model, const input_files& input_file_paths);

struct exec_query_ctx {
    librdf_query* query = nullptr;
    librdf_query_results* results = nullptr;
    bool success = false;
};

void release_exec_query_ctx(exec_query_ctx* ctx);

using exec_query_result = std::unique_ptr<exec_query_ctx, decltype(&release_exec_query_ctx)>;

exec_query_result exec_query(librdf_world* world, librdf_model* model, const std::string& query);

using binding_name = std::string;
using binding_value = std::string;
using data_row = std::map<binding_name, binding_value>;
using data_table = std::vector<data_row>;
using head_row = std::vector<binding_name>;


using extract_cb = std::string (*)(librdf_node*);
using extract_cb_lut = std::map<std::string, extract_cb>;

using extract_data_table_result = std::tuple<head_row, data_table>;


/**
 * @brief Extract the query results and prepare them for further presentation
 */
extract_data_table_result extract_data_table(librdf_query_results* results);

/**
 * @brief Extract the query results and prepare them for further presentation
 */
extract_data_table_result extract_data_table(
    librdf_query_results* results, const extract_cb_lut& cb_lut);

data_row::const_iterator get_binding_value_req(
    const data_row& row,
    const std::string& binding_name);

bool has_binding(const data_row& row, const std::string& binding_name);

void print_data_table(const extract_data_table_result& data_table);

//void exec_query(librdf_world* world, librdf_model* model, const std::string& query_text);

#endif // !defined COMMON_REDLAND_UTILS_HPP
