#if !defined TEST_TOOLS_MATCHERS_HPP
#define TEST_TOOLS_MATCHERS_HPP

#include <gmock/gmock.h>

namespace test::tools
{

/// Dereference both sides and pass them to the inner matcher
MATCHER_P(PointeeBoth, inner_matcher, "matches")
{
    const auto& lhs = std::get<0>(arg);
    const auto& rhs = std::get<1>(arg);

    using PointeeTuple = std::tuple<decltype(*lhs), decltype(*rhs)>;

    ::testing::Matcher<PointeeTuple> tuple_matcher =
          ::testing::SafeMatcherCast<PointeeTuple>(inner_matcher);

    return tuple_matcher.MatchAndExplain(PointeeTuple(*lhs, *rhs), result_listener);
}

/// Pass the dereferenced left side, together with the unchanged right side, to the inner matcher
MATCHER_P(PointeeLeft, inner_matcher, "matches")
{
    const auto& lhs = std::get<0>(arg);
    const auto& rhs = std::get<1>(arg);

    using PointeeTuple = std::tuple<decltype(*lhs), decltype(rhs)>;

    ::testing::Matcher<PointeeTuple> tuple_matcher =
          ::testing::SafeMatcherCast<PointeeTuple>(inner_matcher);

    return tuple_matcher.MatchAndExplain(PointeeTuple(*lhs, rhs), result_listener);
}

/// Pass the unchanged left side, together with the dereferenced right side, to the inner matcher
MATCHER_P(PointeeRight, inner_matcher, "matches")
{
    const auto& lhs = std::get<0>(arg);
    const auto& rhs = std::get<1>(arg);

    using PointeeTuple = std::tuple<decltype(*lhs), decltype(rhs)>;

    ::testing::Matcher<PointeeTuple> tuple_matcher =
          ::testing::SafeMatcherCast<PointeeTuple>(inner_matcher);

    return tuple_matcher.MatchAndExplain(PointeeTuple(*lhs, rhs), result_listener);
}

} // namespace test::tools

#endif // !defined TEST_TOOLS_MATCHERS_HPP
