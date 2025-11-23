#if !defined COMMON_STRING_HPP
#define COMMON_STRING_HPP

#include <sstream>
#include <string>
#include <ranges>

namespace common
{

template <std::ranges::input_range RangeType>
requires requires(std::ostringstream& os, const std::ranges::range_reference_t<RangeType> v) {
    os << v; // the range elements must be insertable into output streams
}
std::string join(const RangeType& range, const std::string& sep = ", ")
{
    std::ostringstream oss;

    for (bool first=true; const auto& value : range)
    {
        if (!first)
        {
            oss << sep;
        }
        else
        {
            first = false;
        }

        oss << value;
    }

    return oss.str();
}

} // namespace common

#endif // !defined COMMON_STRING_HPP
