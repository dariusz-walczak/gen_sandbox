#include "test/tools/resource.hpp"

#include <sstream>

#include <gtest/gtest.h>

#include "test/tools/string.hpp"

namespace test::tools
{

ComparableResource to_comparable(const common::Resource& resource)
{
    return ComparableResource{resource.get_uri_str(), resource.get_caption()};
}

std::vector<ComparableResource> to_comparable(
    const std::vector<std::shared_ptr<common::Resource>>& resource_seq)
{
    std::vector<ComparableResource> output;
    output.reserve(resource_seq.size());

    for (const auto& resource : resource_seq)
    {
        output.emplace_back(to_comparable(*resource));
    }

    return output;
}

std::string to_string(const ComparableResource& res, std::uint8_t depth) noexcept
{
    const std::string indent = construct_indent(depth);

    std::ostringstream oss;
    oss << indent << "ComparableResource{"
        << "id=" << ::testing::PrintToString(res.id) << ", "
        << "caption=" << ::testing::PrintToString(res.caption) << "}";
    return oss.str();
}

} // namespace test::tools
