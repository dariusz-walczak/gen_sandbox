#if !defined GEN_SANDBOX__REDLAND_UTILS_HPP
#define GEN_SANDBOX__REDLAND_UTILS_HPP

#include <string>

#include <redland.h>

void load_rdf(librdf_world* world, librdf_model* model, const std::string& input_file_path);

void exec_query(librdf_world* world, librdf_model* model, const std::string& query_text);

#endif // !defined GEN_SANDBOX__REDLAND_UTILS_HPP
