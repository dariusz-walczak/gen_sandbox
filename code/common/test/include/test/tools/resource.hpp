#if !defined TEST_TOOLS_RESOURCE_HPP
#define TEST_TOOLS_RESOURCE_HPP

#include <cstdint>
#include <string>

namespace test::tools
{

struct ComparableResource
{
    std::string id;
    std::string caption;

    bool operator==(const ComparableResource& other) const {
        return ((id == other.id) && (caption == other.caption));
    }
};

[[nodiscard]] std::string to_string(const ComparableResource& res, std::uint8_t depth=0) noexcept;

inline void PrintTo(const ComparableResource& res, std::ostream* os)
{
    *os << to_string(res);
}

} // namespace test::tools

#endif // !defined TEST_TOOLS_RESOURCE_HPP
