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
const char* file6 { "fake/6.ttl" };
const char* file7 { "fake/7.ttl" };
const char* file8 { "fake/8.ttl" };
const char* file9 { "fake/9.ttl" };

const common::Resource person1 { "http://example.org/P1" };
const common::Resource person2 { "http://example.org/P2" };
const common::Resource person3 { "http://example.org/P3" };
const common::Resource person4 { "http://example.org/P4" };
const common::Resource person5 { "http://example.org/P5" };
const common::Resource person6 { "http://example.org/P6" };
const common::Resource person7 { "http://example.org/P7" };
const common::Resource person8 { "http://example.org/P8" };
const common::Resource person9 { "http://example.org/P9" };

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
    },
    {
        .case_name = "EmptyFileDeps",
        .person_deps = {
            { person1, { person2 } },
            { person2, { person1, person3 } },
            { person3, { person2 } }
        },
        .file_deps = {},
        .expected_deps = {}
    },
    {
        .case_name = "EmptyPersonDeps",
        .person_deps = {},
        .file_deps = {
            { person1, { file1 } },
            { person2, { file2, file3 } },
            { person3, { file4, file5 } },
            { person4, { file5 } }
        },
        .expected_deps = {
            { person1, { file1 } },
            { person2, { file2, file3 } },
            { person3, { file4, file5 } },
            { person4, { file5 } }
        }
    },
    {
        .case_name = "SimpleTwoWayPersonDep",
        .person_deps = {
            { person1, { person2 } },
            { person2, { person1 } }
        },
        .file_deps = {
            { person1, { file1 } },
            { person2, { file2, file3 } }
        },
        .expected_deps = {
            { person1, { file1, file2, file3 } },
            { person2, { file1, file2, file3 } }
        }
    },
    {
        .case_name = "SimpleOneWayPersonDep",
        .person_deps = {
            { person1, { person2 } }
        },
        .file_deps = {
            { person1, { file1 } },
            { person2, { file2, file3 } }
        },
        .expected_deps = {
            { person1, { file1, file2, file3 } },
            { person2, { file2, file3 } }
        }
    },
    {
        .case_name = "MultipleOneWayDeps",
        .person_deps = {
            { person1, { person2, person3, person4 } }
        },
        .file_deps = {
            { person1, { file1 } },
            { person2, { file2 } },
            { person3, { file2, file3 } },
            { person4, { file3, file4, file5 } }
        },
        .expected_deps = {
            { person1, { file1, file2, file3, file4, file5 } },
            { person2, { file2 } },
            { person3, { file2, file3 } },
            { person4, { file3, file4, file5 } }
        }
    },
    {
        .case_name = "TwoPlusTwoFamilyDeps",
        .person_deps = {
            { person1, { person2, person3, person4 } }, // father
            { person2, { person1, person3, person4 } }, // mother
            { person3, { person1, person2 } }, // child1
            { person4, { person1, person2 } } // child2
        },
        .file_deps = {
            { person1, { file1 } },
            { person2, { file2 } },
            { person3, { file3 } },
            { person4, { file4 } }
        },
        .expected_deps = {
            { person1, { file1, file2, file3, file4 } },
            { person2, { file1, file2, file3, file4 } },
            { person3, { file1, file2, file3 } },
            { person4, { file1, file2, file4 } }
        }
    },
    {
        .case_name = "SimpleThreeGenFamilyDeps",
        .person_deps = {
            { person1, { person2, person5 } }, // father's father
            { person2, { person1, person5 } }, // father's mother
            { person3, { person4, person6 } }, // mother's father
            { person4, { person3, person6 } }, // mother's mother
            { person5, { person1, person2, person6, person7, person8 } }, // father
            { person6, { person3, person4, person5, person7, person8 } }, // mother
            { person7, { person5, person6 } }, // child1
            { person8, { person5, person6 } } // child2
        },
        .file_deps = {
            { person1, { file1 } },
            { person2, { file2 } },
            { person3, { file3 } },
            { person4, { file4 } },
            { person5, { file5 } },
            { person6, { file6 } },
            { person7, { file7 } },
            { person8, { file8 } }
        },
        .expected_deps = {
            { person1, { file1, file2, file5 } },
            { person2, { file1, file2, file5 } },
            { person3, { file3, file4, file6 } },
            { person4, { file3, file4, file6 } },
            { person5, { file1, file2, file5, file6, file7, file8 } },
            { person6, { file3, file4, file5, file6, file7, file8 } },
            { person7, { file5, file6, file7 } },
            { person8, { file5, file6, file8 } }
        }
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
