#if !defined TEST_TOOLS_STRING_HPP
#define TEST_TOOLS_STRING_HPP

#include <cstdint>
#include <string>

namespace test::tools
{

[[nodiscard]] inline std::string construct_indent(std::uint8_t depth)
{
    using size_type = std::string::size_type;
    return std::string(static_cast<size_type>(depth)*size_type{2}, ' ');
}

} // namespace test::tools

#endif // !defined TEST_TOOLS_STRING_HPP
