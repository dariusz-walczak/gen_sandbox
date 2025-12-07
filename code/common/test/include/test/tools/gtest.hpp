#if !defined TEST_TOOLS_PRINTERS_HPP
#define TEST_TOOLS_PRINTERS_HPP

#include <gtest/gtest.h>

namespace test::tools
{

template <typename ParamType>
std::string ParamNameGen(const ::testing::TestParamInfo<ParamType>& info)
{
    return { info.param.case_name };
}

} // namespace test::tools

#endif // !defined TEST_TOOLS_PRINTERS_HPP
