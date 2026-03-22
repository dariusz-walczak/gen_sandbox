#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "person/queries/details.hpp"

#include "test/tools/application.hpp"
#include "test/tools/gtest.hpp"
#include "test/tools/redland.hpp"

#include "test/tools/person/comparable_note_factory.hpp"

namespace test::suite_retrieve_person_invalid_parent_notes
{

struct Param
{
    const char* case_name;
    const char* data_file;
    const char* proband_uri;

    std::vector<tools::ComparableNote> expected_notes;
};

class DetailsQueries_RetrievePersonInvalidParentNotes : public ::testing::TestWithParam<Param> {};

TEST_P(DetailsQueries_RetrievePersonInvalidParentNotes, NormalSuccessCases)
{
    const Param& param = GetParam();
    tools::scoped_redland_ctx ctx = tools::initialize_redland_ctx();
    tools::load_rdf(ctx->world, ctx->model, tools::get_program_path() / param.data_file);

    const auto proband = std::make_shared<common::Person>(param.proband_uri);
    std::vector<common::Note> actual_notes;

    person::retrieve_person_invalid_parent_notes(
        proband.get(), ctx->world, ctx->model, actual_notes);

    EXPECT_THAT(
        tools::to_comparable(actual_notes),
        ::testing::UnorderedElementsAreArray(param.expected_notes));
};

using namespace test::tools::person;

const std::vector<Param> g_params {
    {
        .case_name="UnknownParent",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-01_single-parent.ttl"),
        .proband_uri="http://example.org/V101P1",
        .expected_notes={}
    },
    {
        .case_name="StubbedParent",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-01_single-parent.ttl"),
        .proband_uri="http://example.org/V102P1",
        .expected_notes={
            create_stubbed_parent_comparable_note({"http://example.org/V102P2", ""})
        }
    },
    {
        .case_name="UntypedParentUnknown1",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-01_single-parent.ttl"),
        .proband_uri="http://example.org/V103P1",
        .expected_notes={
            create_untyped_parent_comparable_note({"http://example.org/V103P2", ""}),
            create_unknown_parent_gender_comparable_note({"http://example.org/V103P2", ""})
        }
    },
    {
        .case_name="UntypedParentUnknown2",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-01_single-parent.ttl"),
        .proband_uri="http://example.org/V104P1",
        .expected_notes={
            create_untyped_parent_comparable_note({"http://example.org/V104P2", ""}),
            create_unknown_parent_gender_comparable_note({"http://example.org/V104P2", ""})
        }
    },
    {
        .case_name="UntypedParentUnknown3",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-01_single-parent.ttl"),
        .proband_uri="http://example.org/V105P1",
        .expected_notes={
            create_untyped_parent_comparable_note({"http://example.org/V105P2", ""}),
            create_unknown_parent_gender_comparable_note({"http://example.org/V105P2", ""})
        }
    },
    {
        .case_name="UntypedParentUnknown4",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-01_single-parent.ttl"),
        .proband_uri="http://example.org/V106P1",
        .expected_notes={
            create_untyped_parent_comparable_note({"http://example.org/V106P2", ""}),
            create_unknown_parent_gender_comparable_note({"http://example.org/V106P2", ""})
        }
    },
    {
        .case_name="UntypedParentInvalid",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-01_single-parent.ttl"),
        .proband_uri="http://example.org/V107P1",
        .expected_notes={
            create_untyped_parent_comparable_note({"http://example.org/V107P2", ""}),
            create_invalid_parent_gender_comparable_note({"http://example.org/V107P2", ""})
        }
    },
    {
        .case_name="UntypedParentValid",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-01_single-parent.ttl"),
        .proband_uri="http://example.org/V108P1",
        .expected_notes={
            create_untyped_parent_comparable_note({"http://example.org/V108P2", ""}),
        }
    },
    {
        .case_name="MistypedParentUnknown1",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-01_single-parent.ttl"),
        .proband_uri="http://example.org/V109P1",
        .expected_notes={
            create_mistyped_parent_comparable_note({"http://example.org/V109P2", ""}),
            create_unknown_parent_gender_comparable_note({"http://example.org/V109P2", ""})
        }
    },
    {
        .case_name="MistypedParentUnknown2",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-01_single-parent.ttl"),
        .proband_uri="http://example.org/V110P1",
        .expected_notes={
            create_mistyped_parent_comparable_note({"http://example.org/V110P2", ""}),
            create_unknown_parent_gender_comparable_note({"http://example.org/V110P2", ""})
        }
    },
    {
        .case_name="MistypedParentUnknown3",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-01_single-parent.ttl"),
        .proband_uri="http://example.org/V111P1",
        .expected_notes={
            create_mistyped_parent_comparable_note({"http://example.org/V111P2", ""}),
            create_unknown_parent_gender_comparable_note({"http://example.org/V111P2", ""})
        }
    },
    {
        .case_name="MistypedParentUnknown4",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-01_single-parent.ttl"),
        .proband_uri="http://example.org/V112P1",
        .expected_notes={
            create_mistyped_parent_comparable_note({"http://example.org/V112P2", ""}),
            create_unknown_parent_gender_comparable_note({"http://example.org/V112P2", ""})
        }
    },
    {
        .case_name="MistypedParentInvalid",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-01_single-parent.ttl"),
        .proband_uri="http://example.org/V113P1",
        .expected_notes={
            create_mistyped_parent_comparable_note({"http://example.org/V113P2", ""}),
            create_invalid_parent_gender_comparable_note({"http://example.org/V113P2", ""})
        }
    },
    {
        .case_name="MistypedParentValid",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-01_single-parent.ttl"),
        .proband_uri="http://example.org/V114P1",
        .expected_notes={
            create_mistyped_parent_comparable_note({"http://example.org/V114P2", ""}),
        }
    },
    {
        .case_name="TypedParentUnknown1",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-01_single-parent.ttl"),
        .proband_uri="http://example.org/V115P1",
        .expected_notes={
            create_unknown_parent_gender_comparable_note({"http://example.org/V115P2", ""})
        }
    },
    {
        .case_name="TypedParentUnknown2",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-01_single-parent.ttl"),
        .proband_uri="http://example.org/V116P1",
        .expected_notes={
            create_unknown_parent_gender_comparable_note({"http://example.org/V116P2", ""})
        }
    },
    {
        .case_name="TypedParentUnknown3",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-01_single-parent.ttl"),
        .proband_uri="http://example.org/V117P1",
        .expected_notes={
            create_unknown_parent_gender_comparable_note({"http://example.org/V117P2", ""})
        }
    },
    {
        .case_name="TypedParentUnknown4",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-01_single-parent.ttl"),
        .proband_uri="http://example.org/V118P1",
        .expected_notes={
            create_unknown_parent_gender_comparable_note({"http://example.org/V118P2", ""})
        }
    },
    {
        .case_name="TypedParentInvalid",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-01_single-parent.ttl"),
        .proband_uri="http://example.org/V119P1",
        .expected_notes={
            create_invalid_parent_gender_comparable_note({"http://example.org/V119P2", ""})
        }
    },
    {
        .case_name="TypedParentValid",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-01_single-parent.ttl"),
        .proband_uri="http://example.org/V120P1",
        .expected_notes={}
    },
    {
        .case_name="MotherStubbedFatherStubbed",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-02_both-parents.ttl"),
        .proband_uri="http://example.org/V201P1",
        .expected_notes={
            create_stubbed_parent_comparable_note({"http://example.org/V201P2", ""}),
            create_stubbed_parent_comparable_note({"http://example.org/V201P3", ""})
        }
    },
    {
        .case_name="MotherStubbedFatherUntypedUnknown",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-02_both-parents.ttl"),
        .proband_uri="http://example.org/V202P1",
        .expected_notes={
            create_stubbed_parent_comparable_note({"http://example.org/V202P2", ""}),
            create_untyped_parent_comparable_note({"http://example.org/V202P3", ""}),
            create_unknown_parent_gender_comparable_note({"http://example.org/V202P3", ""})
        }
    },
    {
        .case_name="MotherStubbedFatherUntypedInvalid",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-02_both-parents.ttl"),
        .proband_uri="http://example.org/V203P1",
        .expected_notes={
            create_stubbed_parent_comparable_note({"http://example.org/V203P2", ""}),
            create_untyped_parent_comparable_note({"http://example.org/V203P3", ""}),
            create_invalid_parent_gender_comparable_note({"http://example.org/V203P3", ""})
        }
    },
    {
        .case_name="MotherStubbedFatherUntypedValid",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-02_both-parents.ttl"),
        .proband_uri="http://example.org/V204P1",
        .expected_notes={
            create_stubbed_parent_comparable_note({"http://example.org/V204P2", ""}),
            create_untyped_parent_comparable_note({"http://example.org/V204P3", ""})
        }
    },
    {
        .case_name="MotherStubbedFatherTypedUnknown",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-02_both-parents.ttl"),
        .proband_uri="http://example.org/V205P1",
        .expected_notes={
            create_stubbed_parent_comparable_note({"http://example.org/V205P2", ""}),
            create_unknown_parent_gender_comparable_note({"http://example.org/V205P3", ""})
        }
    },
    {
        .case_name="MotherStubbedFatherTypedInvalid",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-02_both-parents.ttl"),
        .proband_uri="http://example.org/V206P1",
        .expected_notes={
            create_stubbed_parent_comparable_note({"http://example.org/V206P2", ""}),
            create_invalid_parent_gender_comparable_note({"http://example.org/V206P3", ""})
        }
    },
    {
        .case_name="MotherStubbedFatherTypedValid",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-02_both-parents.ttl"),
        .proband_uri="http://example.org/V207P1",
        .expected_notes={
            create_stubbed_parent_comparable_note({"http://example.org/V207P2", ""})
        }
    },
    {
        .case_name="MotherUntypedUnknownFatherUntypedUnknown",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-02_both-parents.ttl"),
        .proband_uri="http://example.org/V208P1",
        .expected_notes={
            create_untyped_parent_comparable_note({"http://example.org/V208P2", ""}),
            create_unknown_parent_gender_comparable_note({"http://example.org/V208P2", ""}),
            create_untyped_parent_comparable_note({"http://example.org/V208P3", ""}),
            create_unknown_parent_gender_comparable_note({"http://example.org/V208P3", ""})
        }
    },
    {
        .case_name="MotherUntypedUnknownFatherUntypedInvalid",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-02_both-parents.ttl"),
        .proband_uri="http://example.org/V209P1",
        .expected_notes={
            create_untyped_parent_comparable_note({"http://example.org/V209P2", ""}),
            create_unknown_parent_gender_comparable_note({"http://example.org/V209P2", ""}),
            create_untyped_parent_comparable_note({"http://example.org/V209P3", ""}),
            create_invalid_parent_gender_comparable_note({"http://example.org/V209P3", ""})
        }
    },
    {
        .case_name="MotherUntypedUnknownFatherUntypedValid",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-02_both-parents.ttl"),
        .proband_uri="http://example.org/V210P1",
        .expected_notes={
            create_untyped_parent_comparable_note({"http://example.org/V210P2", ""}),
            create_unknown_parent_gender_comparable_note({"http://example.org/V210P2", ""}),
            create_untyped_parent_comparable_note({"http://example.org/V210P3", ""})
        }
    },
    {
        .case_name="MotherUntypedUnknownFatherTypedUnknown",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-02_both-parents.ttl"),
        .proband_uri="http://example.org/V211P1",
        .expected_notes={
            create_untyped_parent_comparable_note({"http://example.org/V211P2", ""}),
            create_unknown_parent_gender_comparable_note({"http://example.org/V211P2", ""}),
            create_unknown_parent_gender_comparable_note({"http://example.org/V211P3", ""})
        }
    },
    {
        .case_name="MotherUntypedUnknownFatherTypedInvalid",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-02_both-parents.ttl"),
        .proband_uri="http://example.org/V212P1",
        .expected_notes={
            create_untyped_parent_comparable_note({"http://example.org/V212P2", ""}),
            create_unknown_parent_gender_comparable_note({"http://example.org/V212P2", ""}),
            create_invalid_parent_gender_comparable_note({"http://example.org/V212P3", ""})
        }
    },
    {
        .case_name="MotherUntypedUnknownFatherTypedValid",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-02_both-parents.ttl"),
        .proband_uri="http://example.org/V213P1",
        .expected_notes={
            create_untyped_parent_comparable_note({"http://example.org/V213P2", ""}),
            create_unknown_parent_gender_comparable_note({"http://example.org/V213P2", ""})
        }
    },
    {
        .case_name="MotherUntypedInvalidFatherUntypedInvalid",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-02_both-parents.ttl"),
        .proband_uri="http://example.org/V214P1",
        .expected_notes={
            create_untyped_parent_comparable_note({"http://example.org/V214P2", ""}),
            create_invalid_parent_gender_comparable_note({"http://example.org/V214P2", ""}),
            create_untyped_parent_comparable_note({"http://example.org/V214P3", ""}),
            create_invalid_parent_gender_comparable_note({"http://example.org/V214P3", ""})
        }
    },
    {
        .case_name="MotherUntypedInvalidFatherUntypedValid",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-02_both-parents.ttl"),
        .proband_uri="http://example.org/V215P1",
        .expected_notes={
            create_untyped_parent_comparable_note({"http://example.org/V215P2", ""}),
            create_invalid_parent_gender_comparable_note({"http://example.org/V215P2", ""}),
            create_untyped_parent_comparable_note({"http://example.org/V215P3", ""})
        }
    },
    {
        .case_name="MotherUntypedInvalidFatherTypedUnknown",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-02_both-parents.ttl"),
        .proband_uri="http://example.org/V216P1",
        .expected_notes={
            create_untyped_parent_comparable_note({"http://example.org/V216P2", ""}),
            create_invalid_parent_gender_comparable_note({"http://example.org/V216P2", ""}),
            create_unknown_parent_gender_comparable_note({"http://example.org/V216P3", ""})
        }
    },
    {
        .case_name="MotherUntypedInvalidFatherTypedInvalid",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-02_both-parents.ttl"),
        .proband_uri="http://example.org/V217P1",
        .expected_notes={
            create_untyped_parent_comparable_note({"http://example.org/V217P2", ""}),
            create_invalid_parent_gender_comparable_note({"http://example.org/V217P2", ""}),
            create_invalid_parent_gender_comparable_note({"http://example.org/V217P3", ""})
        }
    },
    {
        .case_name="MotherUntypedInvalidFatherTypedValid",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-02_both-parents.ttl"),
        .proband_uri="http://example.org/V218P1",
        .expected_notes={
            create_untyped_parent_comparable_note({"http://example.org/V218P2", ""}),
            create_invalid_parent_gender_comparable_note({"http://example.org/V218P2", ""})
        }
    },
    {
        .case_name="MotherUntypedValidFatherUntypedValid",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-02_both-parents.ttl"),
        .proband_uri="http://example.org/V219P1",
        .expected_notes={
            create_untyped_parent_comparable_note({"http://example.org/V219P2", ""}),
            create_untyped_parent_comparable_note({"http://example.org/V219P3", ""})
        }
    },
    {
        .case_name="MotherUntypedValidFatherTypedUnknown",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-02_both-parents.ttl"),
        .proband_uri="http://example.org/V220P1",
        .expected_notes={
            create_untyped_parent_comparable_note({"http://example.org/V220P2", ""}),
            create_unknown_parent_gender_comparable_note({"http://example.org/V220P3", ""})
        }
    },
    {
        .case_name="MotherUntypedValidFatherTypedInvalid",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-02_both-parents.ttl"),
        .proband_uri="http://example.org/V221P1",
        .expected_notes={
            create_untyped_parent_comparable_note({"http://example.org/V221P2", ""}),
            create_invalid_parent_gender_comparable_note({"http://example.org/V221P3", ""})
        }
    },
    {
        .case_name="MotherUntypedValidFatherTypedValid",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-02_both-parents.ttl"),
        .proband_uri="http://example.org/V222P1",
        .expected_notes={
            create_untyped_parent_comparable_note({"http://example.org/V222P2", ""}),
        }
    },
    {
        .case_name="MotherTypedUnknownFatherTypedUnknown",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-02_both-parents.ttl"),
        .proband_uri="http://example.org/V223P1",
        .expected_notes={
            create_unknown_parent_gender_comparable_note({"http://example.org/V223P2", ""}),
            create_unknown_parent_gender_comparable_note({"http://example.org/V223P3", ""})
        }
    },
    {
        .case_name="MotherTypedUnknownFatherTypedInvalid",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-02_both-parents.ttl"),
        .proband_uri="http://example.org/V224P1",
        .expected_notes={
            create_unknown_parent_gender_comparable_note({"http://example.org/V224P2", ""}),
            create_invalid_parent_gender_comparable_note({"http://example.org/V224P3", ""})
        }
    },
    {
        .case_name="MotherTypedUnknownFatherTypedValid",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-02_both-parents.ttl"),
        .proband_uri="http://example.org/V225P1",
        .expected_notes={
            create_unknown_parent_gender_comparable_note({"http://example.org/V225P2", ""})
        }
    },
    {
        .case_name="MotherTypedInvalidFatherTypedInvalid",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-02_both-parents.ttl"),
        .proband_uri="http://example.org/V226P1",
        .expected_notes={
            create_invalid_parent_gender_comparable_note({"http://example.org/V226P2", ""}),
            create_invalid_parent_gender_comparable_note({"http://example.org/V226P3", ""})
        }
    },
    {
        .case_name="MotherTypedInvalidFatherTypedValid",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-02_both-parents.ttl"),
        .proband_uri="http://example.org/V227P1",
        .expected_notes={
            create_invalid_parent_gender_comparable_note({"http://example.org/V227P2", ""})
        }
    },
    {
        .case_name="MotherTypedValidFatherTypedValid",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-02_both-parents.ttl"),
        .proband_uri="http://example.org/V228P1",
        .expected_notes={}
    },
    {
        .case_name="ThreeParents",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-03_other-cases.ttl"),
        .proband_uri="http://example.org/V301P1",
        .expected_notes={
            create_untyped_parent_comparable_note({"http://example.org/V301P2", ""}),
            create_untyped_parent_comparable_note({"http://example.org/V301P3", ""}),
            create_mistyped_parent_comparable_note({"http://example.org/V301P4", ""}),
        }
    },
    {
        .case_name="NoParents",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-03_other-cases.ttl"),
        .proband_uri="http://example.org/V302P1",
        .expected_notes={}
    },
    {
        .case_name="FourParents",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-03_other-cases.ttl"),
        .proband_uri="http://example.org/V303P1",
        .expected_notes={
            create_unknown_parent_gender_comparable_note({"http://example.org/V303P4", ""}),
            create_unknown_parent_gender_comparable_note({"http://example.org/V303P5", ""}),
        }
    },
    {
        .case_name="BadRelationshipType",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-03_other-cases.ttl"),
        .proband_uri="http://example.org/V304P1",
        .expected_notes={}
    },
    {
        .case_name="NamedParents",
        .data_file=(
            "data/queries/details/retrieve_person_invalid_parent_notes/"
            "model-03_other-cases.ttl"),
        .proband_uri="http://example.org/V305P1",
        .expected_notes={
            create_invalid_parent_gender_comparable_note(
                {"http://example.org/V305P2", "Johansson, Alma"}),
            create_unknown_parent_gender_comparable_note(
                {"http://example.org/V305P3", "Andersson, Lars"})
        }
    },
};

INSTANTIATE_TEST_SUITE_P(
    ,
    DetailsQueries_RetrievePersonInvalidParentNotes,
    ::testing::ValuesIn(g_params),
    tools::ParamNameGen<Param>);

} // namespace test::suite_retrieve_person_invalid_parent_notes
