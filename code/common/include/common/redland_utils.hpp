#if !defined COMMON_REDLAND_UTILS_HPP
#define COMMON_REDLAND_UTILS_HPP

#include <map>
#include <string>

#include <redland.h>
#include <spdlog/spdlog.h>


struct redland_context {
    librdf_world*   world;
    librdf_storage* storage;
    librdf_model*   model;
};

void release_redland_ctx(redland_context* ctx);

typedef
    std::unique_ptr<redland_context, decltype(&release_redland_ctx)>
    scoped_redland_ctx;

scoped_redland_ctx create_redland_ctx();
/** Initialize a new Redland RDF Library context
 *
 * @throws common_exception when the context initialization fails. All Redland resources allocated
 *     before the failure are released automatically. */
void initialize_redland_ctx(scoped_redland_ctx& ctx);


void load_rdf(librdf_world* world, librdf_model* model, const std::string& input_file_path);

struct exec_query_ctx {
    librdf_query* query = nullptr;
    librdf_query_results* results = nullptr;
    bool success = false;
};

void release_exec_query_ctx(exec_query_ctx* ctx);

typedef
    std::unique_ptr<exec_query_ctx, decltype(&release_exec_query_ctx)>
    exec_query_result;

exec_query_result exec_query(librdf_world* world, librdf_model* model, const std::string& query);

typedef std::string binding_name;
typedef std::string binding_value;
typedef std::map<binding_name, binding_value> data_row;
typedef std::vector<data_row> data_table;
typedef std::vector<binding_name> head_row;


typedef std::string (*extract_cb)(librdf_node*);
typedef std::map<std::string, extract_cb> extract_cb_lut;

typedef
    std::tuple<head_row, data_table>
    extract_data_table_result;

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
