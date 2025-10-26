#include "test/tools/resource.hpp"

#include <sstream>

#include <gtest/gtest.h>

#include "test/tools/string.hpp"

namespace test::tools
{

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
