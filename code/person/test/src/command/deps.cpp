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

const common::Resource person1 { "http://example.org/P1" };
const common::Resource person2 { "http://example.org/P2" };
const common::Resource person3 { "http://example.org/P3" };
const common::Resource person4 { "http://example.org/P4" };
const common::Resource person5 { "http://example.org/P5" };
const common::Resource person6 { "http://example.org/P6" };
const common::Resource person7 { "http://example.org/P7" };
const common::Resource person8 { "http://example.org/P8" };

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

#define father_of(x) x
#define mother_of(x) x
#define partner_of(x) x
#define child_of(x) x

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
            { person1, {
                    partner_of(person2),
                    father_of(person3), father_of(person4) } },
            { person2, {
                    partner_of(person1),
                    mother_of(person3), mother_of(person4) } },
            { person3, {
                    child_of(person1), child_of(person2) } },
            { person4, {
                    child_of(person1), child_of(person2) } }
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
            { person1, {
                    partner_of(person2),
                    father_of(person5) } },
            { person2, {
                    partner_of(person1),
                    mother_of(person5) } },
            { person3, {
                    partner_of(person4),
                    father_of(person6) } },
            { person4, {
                    partner_of(person3),
                    mother_of(person6) } },
            { person5, {
                    child_of(person1), child_of(person2),
                    partner_of(person6),
                    father_of(person7), father_of(person8) } },
            { person6, {
                    child_of(person3), child_of(person4),
                    partner_of(person5),
                    mother_of(person7), mother_of(person8) } },
            { person7, {
                    child_of(person5), child_of(person6) } },
            { person8, {
                    child_of(person5), child_of(person6) } }
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
