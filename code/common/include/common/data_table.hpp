#if !defined COMMON_DATA_TABLE_HPP
#define COMMON_DATA_TABLE_HPP

#include <string>
#include <vector>

#include "common/redland_utils.hpp"


namespace common
{

std::vector<std::string> extract_resource_uri_seq(
    const common::data_table& data_table, const std::string& binding_name);

bool get_boolean_value_req(const data_row& row, const std::string& binding_name);

} // namespace common

#endif // !defined COMMON_DATA_TABLE_HPP
