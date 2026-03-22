#include <gtest/gtest.h>

#include "person/error.hpp"
#include "person/queries/details.hpp"

#include "test/tools/application.hpp"
#include "test/tools/assertions.hpp"
#include "test/tools/gtest.hpp"
#include "test/tools/note.hpp"
#include "test/tools/redland.hpp"

//  The retrieve_person_father and retrieve_person_mother functions tests
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

namespace test::suite_retrieve_person_parent
{

struct Param
{
    const char* case_name;
    const char* data_file;
    const char* proband_uri;

    std::optional<common::Person> expected_father;
    std::vector<tools::ComparableNote> expected_father_notes;
    std::optional<common::Person> expected_mother;
    std::vector<tools::ComparableNote> expected_mother_notes;
};

class DetailsQueries_RetrievePersonParents : public ::testing::TestWithParam<Param> {};

TEST_P(DetailsQueries_RetrievePersonParents, NormalSuccessCases)
{
    const Param& param = GetParam();
    tools::scoped_redland_ctx ctx = tools::initialize_redland_ctx();
    tools::load_rdf(ctx->world, ctx->model, tools::get_program_path() / param.data_file);

    const auto proband = std::make_shared<common::Person>(param.proband_uri);

    std::vector<common::Note> actual_father_notes;
    std::vector<common::Note> actual_mother_notes;

    const auto& actual_father = person::retrieve_person_father(
        proband.get(), ctx->world, ctx->model, actual_father_notes);

    const auto& actual_mother = person::retrieve_person_mother(
        proband.get(), ctx->world, ctx->model, actual_mother_notes);

    if (param.expected_father.has_value())
    {
        EXPECT_EQ(param.expected_father.value(), *actual_father);
    }
    else
    {
        EXPECT_EQ(nullptr, actual_father.get());
    }

    EXPECT_EQ(param.expected_father_notes, tools::to_comparable(actual_father_notes));

    if (param.expected_mother.has_value())
    {
        EXPECT_EQ(param.expected_mother.value(), *actual_mother);
    }
    else
    {
        EXPECT_EQ(nullptr, actual_mother.get());
    }

    EXPECT_EQ(param.expected_mother_notes, tools::to_comparable(actual_mother_notes));
};

const std::vector<Param> g_params {
    {
        .case_name="NoParents",
        .data_file=(
            "data/queries/details/retrieve_person_parent/"
            "model-00_no-parents.ttl"),
        .proband_uri="http://example.org/P1",
        .expected_father={},
        .expected_father_notes={},
        .expected_mother={},
        .expected_mother_notes={}
    },
    {
        .case_name="FatherOnly",
        .data_file=(
            "data/queries/details/retrieve_person_parent/"
            "model-01_father-only.ttl"),
        .proband_uri="http://example.org/P1",
        .expected_father={common::Person("http://example.org/P2")},
        .expected_father_notes={},
        .expected_mother={},
        .expected_mother_notes={}
    },
    {
        .case_name="MotherOnly",
        .data_file=(
            "data/queries/details/retrieve_person_parent/"
            "model-02_mother-only.ttl"),
        .proband_uri="http://example.org/P1",
        .expected_father={},
        .expected_father_notes={},
        .expected_mother={common::Person("http://example.org/P3")},
        .expected_mother_notes={}
    },
    {
        .case_name="BothParents",
        .data_file=(
            "data/queries/details/retrieve_person_parent/"
            "model-03_both-parents.ttl"),
        .proband_uri="http://example.org/P1",
        .expected_father={common::Person("http://example.org/P2")},
        .expected_father_notes={},
        .expected_mother={common::Person("http://example.org/P3")},
        .expected_mother_notes={}
    },
    {
        .case_name="TwoFamiliesTheeGenerationsF1G3P1",
        .data_file=(
            "data/queries/details/retrieve_person_parent/"
            "model-04_two-families-three-generations.ttl"),
        .proband_uri="http://example.org/F1G3P1",
        .expected_father={common::Person("http://example.org/F1G2P1")},
        .expected_father_notes={},
        .expected_mother={common::Person("http://example.org/F1G2P2")},
        .expected_mother_notes={}
    },
    {
        .case_name="TwoFamiliesTheeGenerationsF1G3P2",
        .data_file=(
            "data/queries/details/retrieve_person_parent/"
            "model-04_two-families-three-generations.ttl"),
        .proband_uri="http://example.org/F1G3P2",
        .expected_father={common::Person("http://example.org/F1G2P1")},
        .expected_father_notes={},
        .expected_mother={common::Person("http://example.org/F1G2P2")},
        .expected_mother_notes={}
    },
    {
        .case_name="TwoFamiliesTheeGenerationsF1G3P3",
        .data_file=(
            "data/queries/details/retrieve_person_parent/"
            "model-04_two-families-three-generations.ttl"),
        .proband_uri="http://example.org/F1G3P3",
        .expected_father={common::Person("http://example.org/F1G2P1")},
        .expected_father_notes={},
        .expected_mother={common::Person("http://example.org/F1G2P2")},
        .expected_mother_notes={}
    },
    {
        .case_name="TwoFamiliesTheeGenerationsF1G2P1",
        .data_file=(
            "data/queries/details/retrieve_person_parent/"
            "model-04_two-families-three-generations.ttl"),
        .proband_uri="http://example.org/F1G2P1",
        .expected_father={common::Person("http://example.org/F1G1P1")},
        .expected_father_notes={},
        .expected_mother={common::Person("http://example.org/F1G1P2")},
        .expected_mother_notes={}
    },
    {
        .case_name="TwoFamiliesTheeGenerationsF1G2P2",
        .data_file=(
            "data/queries/details/retrieve_person_parent/"
            "model-04_two-families-three-generations.ttl"),
        .proband_uri="http://example.org/F1G2P2",
        .expected_father={common::Person("http://example.org/F1G1P3")},
        .expected_father_notes={},
        .expected_mother={common::Person("http://example.org/F1G1P4")},
        .expected_mother_notes={}
    },
    {
        .case_name="TwoFamiliesTheeGenerationsF1G1P1",
        .data_file=(
            "data/queries/details/retrieve_person_parent/"
            "model-04_two-families-three-generations.ttl"),
        .proband_uri="http://example.org/F1G1P1",
        .expected_father={},
        .expected_father_notes={},
        .expected_mother={},
        .expected_mother_notes={}
    },
    {
        .case_name="TwoFamiliesTheeGenerationsF1G1P2",
        .data_file=(
            "data/queries/details/retrieve_person_parent/"
            "model-04_two-families-three-generations.ttl"),
        .proband_uri="http://example.org/F1G1P2",
        .expected_father={},
        .expected_father_notes={},
        .expected_mother={},
        .expected_mother_notes={}
    },
    {
        .case_name="TwoFamiliesTheeGenerationsF1G1P3",
        .data_file=(
            "data/queries/details/retrieve_person_parent/"
            "model-04_two-families-three-generations.ttl"),
        .proband_uri="http://example.org/F1G1P3",
        .expected_father={},
        .expected_father_notes={},
        .expected_mother={},
        .expected_mother_notes={}
    },
    {
        .case_name="TwoFamiliesTheeGenerationsF1G1P4",
        .data_file=(
            "data/queries/details/retrieve_person_parent/"
            "model-04_two-families-three-generations.ttl"),
        .proband_uri="http://example.org/F1G1P4",
        .expected_father={},
        .expected_father_notes={},
        .expected_mother={},
        .expected_mother_notes={}
    },
    {
        .case_name="TwoFamiliesTheeGenerationsF2G3P1",
        .data_file=(
            "data/queries/details/retrieve_person_parent/"
            "model-04_two-families-three-generations.ttl"),
        .proband_uri="http://example.org/F2G3P1",
        .expected_father={common::Person("http://example.org/F2G2P1")},
        .expected_father_notes={},
        .expected_mother={common::Person("http://example.org/F2G2P2")},
        .expected_mother_notes={}
    },
    {
        .case_name="TwoFamiliesTheeGenerationsF2G3P2",
        .data_file=(
            "data/queries/details/retrieve_person_parent/"
            "model-04_two-families-three-generations.ttl"),
        .proband_uri="http://example.org/F2G3P2",
        .expected_father={common::Person("http://example.org/F2G2P1")},
        .expected_father_notes={},
        .expected_mother={common::Person("http://example.org/F2G2P2")},
        .expected_mother_notes={}
    },
    {
        .case_name="TwoFamiliesTheeGenerationsF2G3P3",
        .data_file=(
            "data/queries/details/retrieve_person_parent/"
            "model-04_two-families-three-generations.ttl"),
        .proband_uri="http://example.org/F2G3P3",
        .expected_father={common::Person("http://example.org/F2G2P3")},
        .expected_father_notes={},
        .expected_mother={common::Person("http://example.org/F2G2P4")},
        .expected_mother_notes={}
    },
    {
        .case_name="TwoFamiliesTheeGenerationsF2G2P1",
        .data_file=(
            "data/queries/details/retrieve_person_parent/"
            "model-04_two-families-three-generations.ttl"),
        .proband_uri="http://example.org/F2G2P1",
        .expected_father={common::Person("http://example.org/F2G1P1")},
        .expected_father_notes={},
        .expected_mother={common::Person("http://example.org/F2G1P2")},
        .expected_mother_notes={}
    },
    {
        .case_name="TwoFamiliesTheeGenerationsF2G2P2",
        .data_file=(
            "data/queries/details/retrieve_person_parent/"
            "model-04_two-families-three-generations.ttl"),
        .proband_uri="http://example.org/F2G2P2",
        .expected_father={common::Person("http://example.org/F2G1P3")},
        .expected_father_notes={},
        .expected_mother={common::Person("http://example.org/F2G1P4")},
        .expected_mother_notes={}
    },
    {
        .case_name="TwoFamiliesTheeGenerationsF2G2P3",
        .data_file=(
            "data/queries/details/retrieve_person_parent/"
            "model-04_two-families-three-generations.ttl"),
        .proband_uri="http://example.org/F2G2P3",
        .expected_father={common::Person("http://example.org/F2G1P3")},
        .expected_father_notes={},
        .expected_mother={common::Person("http://example.org/F2G1P4")},
        .expected_mother_notes={}
    },
    {
        .case_name="TwoFamiliesTheeGenerationsF2G2P4",
        .data_file=(
            "data/queries/details/retrieve_person_parent/"
            "model-04_two-families-three-generations.ttl"),
        .proband_uri="http://example.org/F2G2P4",
        .expected_father={common::Person("http://example.org/F2G1P5")},
        .expected_father_notes={},
        .expected_mother={common::Person("http://example.org/F2G1P6")},
        .expected_mother_notes={}
    },
    {
        .case_name="TwoFamiliesTheeGenerationsF2G1P1",
        .data_file=(
            "data/queries/details/retrieve_person_parent/"
            "model-04_two-families-three-generations.ttl"),
        .proband_uri="http://example.org/F2G1P1",
        .expected_father={},
        .expected_father_notes={},
        .expected_mother={},
        .expected_mother_notes={}
    },
    {
        .case_name="TwoFamiliesTheeGenerationsF2G1P2",
        .data_file=(
            "data/queries/details/retrieve_person_parent/"
            "model-04_two-families-three-generations.ttl"),
        .proband_uri="http://example.org/F2G1P2",
        .expected_father={},
        .expected_father_notes={},
        .expected_mother={},
        .expected_mother_notes={}
    },
    {
        .case_name="TwoFamiliesTheeGenerationsF2G1P3",
        .data_file=(
            "data/queries/details/retrieve_person_parent/"
            "model-04_two-families-three-generations.ttl"),
        .proband_uri="http://example.org/F2G1P3",
        .expected_father={},
        .expected_father_notes={},
        .expected_mother={},
        .expected_mother_notes={}
    },
    {
        .case_name="TwoFamiliesTheeGenerationsF2G1P4",
        .data_file=(
            "data/queries/details/retrieve_person_parent/"
            "model-04_two-families-three-generations.ttl"),
        .proband_uri="http://example.org/F2G1P4",
        .expected_father={},
        .expected_father_notes={},
        .expected_mother={},
        .expected_mother_notes={}
    },
    {
        .case_name="TwoFamiliesTheeGenerationsF2G1P5",
        .data_file=(
            "data/queries/details/retrieve_person_parent/"
            "model-04_two-families-three-generations.ttl"),
        .proband_uri="http://example.org/F2G1P5",
        .expected_father={},
        .expected_father_notes={},
        .expected_mother={},
        .expected_mother_notes={}
    },
    {
        .case_name="TwoFamiliesTheeGenerationsF2G1P6",
        .data_file=(
            "data/queries/details/retrieve_person_parent/"
            "model-04_two-families-three-generations.ttl"),
        .proband_uri="http://example.org/F2G1P6",
        .expected_father={},
        .expected_father_notes={},
        .expected_mother={},
        .expected_mother_notes={}
    },
    {
        .case_name="TwoFathersTwoMothersErrors",
        .data_file=(
            "data/queries/details/retrieve_person_parent/"
            "model-05_multiple-resources-found.ttl"),
        .proband_uri="http://example.org/P1",
        .expected_father={},
        .expected_father_notes={
            {
                .type=common::Note::Type::Error,
                .id=std::string(person::k_multiple_fathers_note_id),
                .vars={
                    tools::ComparableVariable{
                        .name="fathers",
                        .value=std::vector<tools::ComparableVariable>{
                            {
                                tools::ComparableVariable{
                                    .name="",
                                    .value={tools::ComparableResource{"http://example.org/P2", ""}}
                                }
                            },
                            {
                                tools::ComparableVariable{
                                    .name="",
                                    .value={tools::ComparableResource{"http://example.org/P4", ""}}
                                }
                            }
                        }
                    }
                },
                .diagnostic_text=(
                    "Multiple fathers found:\n"
                    "    http://example.org/P2\n"
                    "    http://example.org/P4")
            }
        },
        .expected_mother={},
        .expected_mother_notes={
            {
                .type=common::Note::Type::Error,
                .id=std::string(person::k_multiple_mothers_note_id),
                .vars={
                    tools::ComparableVariable{
                        .name="mothers",
                        .value=std::vector<tools::ComparableVariable>{
                            {
                                tools::ComparableVariable{
                                    .name="",
                                    .value={tools::ComparableResource{"http://example.org/P3", ""}}
                                }
                            },
                            {
                                tools::ComparableVariable{
                                    .name="",
                                    .value={tools::ComparableResource{"http://example.org/P5", ""}}
                                }
                            }
                        }
                    }
                },
                .diagnostic_text=(
                    "Multiple mothers found:\n"
                    "    http://example.org/P3\n"
                    "    http://example.org/P5")
            }
        },
    }
};

INSTANTIATE_TEST_SUITE_P(
    ,
    DetailsQueries_RetrievePersonParents,
    ::testing::ValuesIn(g_params),
    tools::ParamNameGen<Param>);

// Check if the retrieve_person_father_opt and the retrieve_person_mother_opt functions throw the
//  person_exception when any of their arguments are null.
TEST_F(DetailsQueries_RetrievePersonParents, InputContractViolations)
{
    tools::scoped_redland_ctx ctx = tools::initialize_redland_ctx();
    const auto person = std::make_shared<common::Person>("http://example.org/someone");

    std::vector<common::Note> actual_notes;

    EXPECT_THROW_WITH_CODE(
        person::retrieve_person_father(nullptr, ctx->world, ctx->model, actual_notes),
        person::person_exception, person::person_exception::error_code::input_contract_error);
    EXPECT_EQ(std::vector<tools::ComparableNote>{}, tools::to_comparable(actual_notes));

    EXPECT_THROW_WITH_CODE(
        person::retrieve_person_father(person.get(), nullptr, ctx->model, actual_notes),
        person::person_exception, person::person_exception::error_code::input_contract_error);
    EXPECT_EQ(std::vector<tools::ComparableNote>{}, tools::to_comparable(actual_notes));

    EXPECT_THROW_WITH_CODE(
        person::retrieve_person_father(person.get(), ctx->world, nullptr, actual_notes),
        person::person_exception, person::person_exception::error_code::input_contract_error);
    EXPECT_EQ(std::vector<tools::ComparableNote>{}, tools::to_comparable(actual_notes));

    EXPECT_THROW_WITH_CODE(
        person::retrieve_person_mother(nullptr, ctx->world, ctx->model, actual_notes),
        person::person_exception, person::person_exception::error_code::input_contract_error);

    EXPECT_THROW_WITH_CODE(
        person::retrieve_person_mother(person.get(), nullptr, ctx->model, actual_notes),
        person::person_exception, person::person_exception::error_code::input_contract_error);

    EXPECT_THROW_WITH_CODE(
        person::retrieve_person_mother(person.get(), ctx->world, nullptr, actual_notes),
        person::person_exception, person::person_exception::error_code::input_contract_error);
}

} // namespace test::suite_retrieve_person_parent
