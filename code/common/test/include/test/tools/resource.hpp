#if !defined TEST_TOOLS_RESOURCE_HPP
#define TEST_TOOLS_RESOURCE_HPP

#include <cstdint>
#include <memory>
#include <string>

#include "common/resource.hpp"

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

[[nodiscard]] ComparableResource to_comparable(const common::Resource& resource);
[[nodiscard]] std::vector<ComparableResource> to_comparable(
    const std::vector<std::shared_ptr<common::Resource>>& resource_seq);
[[nodiscard]] std::string to_string(const ComparableResource& res, std::uint8_t depth=0) noexcept;

inline void PrintTo(const ComparableResource& res, std::ostream* os)
{
    *os << to_string(res);
}

} // namespace test::tools

#endif // !defined TEST_TOOLS_RESOURCE_HPP
