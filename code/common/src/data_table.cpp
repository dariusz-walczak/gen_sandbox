#include "common/data_table.hpp"

#include <spdlog/spdlog.h>

namespace common
{

std::vector<std::string> extract_resource_uri_seq(
    const common::data_table& data_table, const std::string& binding_name)
{
    spdlog::trace("{}: Entry checkpoint", __func__);

    std::vector<std::string> result_seq;
    result_seq.reserve(data_table.size());

    for (const common::data_row& row : data_table)
    {
        auto uri_it = common::get_binding_value_req(row, binding_name);
        result_seq.emplace_back(uri_it->second);
    }

    return result_seq;
}

} // namespace common
