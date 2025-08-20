#include <map>
#include <set>

#include <gtest/gtest.h>

#include "common/resource.hpp"

//  The merge_dependencies function tests
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

// The merge_dependencies function is not declared in any header, so it has to be introduced here
namespace person::detail
{
using file_deps_lut = std::map<common::Resource, common::file_set>;
using person_deps_lut = std::map<common::Resource, std::set<common::Resource>>;
file_deps_lut merge_dependencies(const person_deps_lut&, const file_deps_lut&);
} // namespace person::detail

namespace test
{

namespace {

const char* file1 { "fake/1.ttl" };
const char* file2 { "fake/2.ttl" };
const char* file3 { "fake/3.ttl" };
const char* file4 { "fake/4.ttl" };
const char* file5 { "fake/5.ttl" };

const common::Resource person1 { "http://example.org/P1" };
const common::Resource person2 { "http://example.org/P2" };
const common::Resource person3 { "http://example.org/P3" };
const common::Resource person4 { "http://example.org/P4" };
const common::Resource person5 { "http://example.org/P5" };

struct Param
{
    const char* case_name;
    person::detail::person_deps_lut person_deps;
    person::detail::file_deps_lut file_deps;
    person::detail::file_deps_lut expected_deps;
};

class DepsCommand_MergeDependencies : public ::testing::TestWithParam<Param> {};

TEST_P(DepsCommand_MergeDependencies, NormalSuccessCases)
{
    const Param& param = GetParam();

    person::detail::file_deps_lut actual_deps =
        person::detail::merge_dependencies(param.person_deps, param.file_deps);

    EXPECT_EQ(param.expected_deps, actual_deps);
};

const std::vector<Param> g_params {
    {
        .case_name = "EmptyInputs",
        .person_deps = {},
        .file_deps = {},
        .expected_deps = {}
    }
};

std::string ParamNameGen(const ::testing::TestParamInfo<Param>& info)
{
    return { info.param.case_name };
}


INSTANTIATE_TEST_SUITE_P(
    ,
    DepsCommand_MergeDependencies,
    ::testing::ValuesIn(g_params),
    ParamNameGen);

} // anonymous namespace

} // namespace test
