#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "person/error.hpp"
#include "person/queries/details.hpp"

#include "test/tools/application.hpp"
#include "test/tools/assertions.hpp"
#include "test/tools/gtest.hpp"
#include "test/tools/person.hpp"
#include "test/tools/redland.hpp"
#include "test/tools/resource.hpp"

namespace test::suite_retrieve_person_parents
{
namespace
{

struct Param
{
    const char* case_name;
    const char* data_file;
    const char* proband_uri;

    std::vector<tools::ComparableResource> expected_parents;
};

class Queries_Details_RetrievePersonParents : public ::testing::TestWithParam<Param> {};

TEST_F(Queries_Details_RetrievePersonParents, AllParentResourceStates)
{
    tools::scoped_redland_ctx ctx = tools::initialize_redland_ctx();
    const char* data_file_path =
        "data/queries/details/retrieve_person_parents/model-01_parent-resource-states.ttl";
    tools::load_rdf(ctx->world, ctx->model, tools::get_program_path() / data_file_path);

    const auto proband = std::make_shared<common::Person>("http://example.org/V101P0");
    const std::vector<tools::ComparableResource> expected_parents {
        {"http://example.org/V101P1", ""}, {"http://example.org/V101P2", ""},
        {"http://example.org/V101P3", ""}, {"http://example.org/V101P4", ""}};

    const std::vector<tools::ComparableResource> actual_parents = tools::to_comparable(
        person::retrieve_person_parents(
            proband.get(), ctx->world, ctx->model));

    EXPECT_THAT(actual_parents, ::testing::UnorderedElementsAreArray(expected_parents));
};

TEST_F(Queries_Details_RetrievePersonParents, AllParentGenderStates)
{
    tools::scoped_redland_ctx ctx = tools::initialize_redland_ctx();
    const char* data_file_path =
        "data/queries/details/retrieve_person_parents/model-02_parent-gender-states.ttl";
    tools::load_rdf(ctx->world, ctx->model, tools::get_program_path() / data_file_path);

    const auto proband = std::make_shared<common::Person>("http://example.org/V102P0");
    const std::vector<tools::ComparableResource> expected_parents {
        {"http://example.org/V102P1", ""}, {"http://example.org/V102P2", ""},
        {"http://example.org/V102P3", ""}, {"http://example.org/V102P4", ""},
        {"http://example.org/V102P5", ""}, {"http://example.org/V102P6", ""},
        {"http://example.org/V102P7", ""}};

    const std::vector<tools::ComparableResource> actual_parents = tools::to_comparable(
        person::retrieve_person_parents(
            proband.get(), ctx->world, ctx->model));

    EXPECT_THAT(actual_parents, ::testing::UnorderedElementsAreArray(expected_parents));
};

TEST_P(Queries_Details_RetrievePersonParents, MultiParentCases)
{
    const Param& param = GetParam();
    tools::scoped_redland_ctx ctx = tools::initialize_redland_ctx();
    tools::load_rdf(ctx->world, ctx->model, tools::get_program_path() / param.data_file);

    const auto proband = std::make_shared<common::Person>(param.proband_uri);

    const std::vector<tools::ComparableResource> actual_parents = tools::to_comparable(
        person::retrieve_person_parents(
            proband.get(), ctx->world, ctx->model));

    EXPECT_THAT(actual_parents, ::testing::UnorderedElementsAreArray(param.expected_parents));
};

const std::vector<Param> g_two_parent_cases {
    {
        .case_name="BothValid",
        .data_file="data/queries/details/retrieve_person_parents/model-03_two-parent-variants.ttl",
        .proband_uri="http://example.org/V201P0",
        .expected_parents={{"http://example.org/V201P1", ""}, {"http://example.org/V201P2", ""}}
    },
    {
        .case_name="OneValidOneStubbed",
        .data_file="data/queries/details/retrieve_person_parents/model-03_two-parent-variants.ttl",
        .proband_uri="http://example.org/V202P0",
        .expected_parents={{"http://example.org/V202P1", ""}, {"http://example.org/V202P2", ""}}
    },
    {
        .case_name="OneValidOneMistyped",
        .data_file="data/queries/details/retrieve_person_parents/model-03_two-parent-variants.ttl",
        .proband_uri="http://example.org/V203P0",
        .expected_parents={{"http://example.org/V203P1", ""}, {"http://example.org/V203P2", ""}}
    },
    {
        .case_name="BothInvalid",
        .data_file="data/queries/details/retrieve_person_parents/model-03_two-parent-variants.ttl",
        .proband_uri="http://example.org/V204P0",
        .expected_parents={{"http://example.org/V204P1", ""}, {"http://example.org/V204P2", ""}}
    }
};

INSTANTIATE_TEST_SUITE_P(
    TwoParentCases,
    Queries_Details_RetrievePersonParents,
    ::testing::ValuesIn(g_two_parent_cases),
    tools::ParamNameGen<Param>);

const std::vector<Param> g_other_success_cases {
    {
        .case_name="ThreeValidSameGender",
        .data_file="data/queries/details/retrieve_person_parents/model-04_other-variants.ttl",
        .proband_uri="http://example.org/V301P0",
        .expected_parents={
            {"http://example.org/V301P1", ""}, {"http://example.org/V301P2", ""},
            {"http://example.org/V301P3", ""}}
    },
    {
        .case_name="FiveParentsMixedValidity",
        .data_file="data/queries/details/retrieve_person_parents/model-04_other-variants.ttl",
        .proband_uri="http://example.org/V302P0",
        .expected_parents={
            {"http://example.org/V302P1", ""}, {"http://example.org/V302P2", ""},
            {"http://example.org/V302P3", ""}, {"http://example.org/V302P4", ""},
            {"http://example.org/V302P5", ""}}
    },
    {
        .case_name="RelationshipTypeFiltering",
        .data_file="data/queries/details/retrieve_person_parents/model-04_other-variants.ttl",
        .proband_uri="http://example.org/V303P0",
        .expected_parents={
            {"http://example.org/V303P1", ""}, {"http://example.org/V303P2", ""}}
    },
    {
        .case_name="ExcludeChildren",
        .data_file="data/queries/details/retrieve_person_parents/model-04_other-variants.ttl",
        .proband_uri="http://example.org/V304P0",
        .expected_parents={{"http://example.org/V304P1", ""}}
    },
    {
        .case_name="EmptyResult",
        .data_file="data/queries/details/retrieve_person_parents/model-04_other-variants.ttl",
        .proband_uri="http://example.org/V305P0",
        .expected_parents={}
    },
    {
        .case_name="RemoveDuplicates",
        .data_file="data/queries/details/retrieve_person_parents/model-04_other-variants.ttl",
        .proband_uri="http://example.org/V306P0",
        .expected_parents={
            {"http://example.org/V306P1", ""}, {"http://example.org/V306P2", ""}}
    }
};

INSTANTIATE_TEST_SUITE_P(
    OtherSuccessCases,
    Queries_Details_RetrievePersonParents,
    ::testing::ValuesIn(g_other_success_cases),
    tools::ParamNameGen<Param>);

// Check if the retrieve_person_parents function throws the person_exception when any of its
//  arguments is null
TEST_F(Queries_Details_RetrievePersonParents, InputContractViolations)
{
    tools::scoped_redland_ctx ctx = tools::initialize_redland_ctx();
    const auto person = std::make_shared<common::Person>("http://example.org/someone");

    EXPECT_THROW_WITH_CODE(
        person::retrieve_person_parents(nullptr, ctx->world, ctx->model),
        person::person_exception, person::person_exception::error_code::input_contract_error);

    EXPECT_THROW_WITH_CODE(
        person::retrieve_person_parents(person.get(), nullptr, ctx->model),
        person::person_exception, person::person_exception::error_code::input_contract_error);

    EXPECT_THROW_WITH_CODE(
        person::retrieve_person_parents(person.get(), ctx->world, nullptr),
        person::person_exception, person::person_exception::error_code::input_contract_error);
}

} // anonymous namespace
} // namespace test::suite_retrieve_person_parents
