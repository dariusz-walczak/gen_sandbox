#include <gtest/gtest.h>

#include "common/person.hpp"
#include "test/tools/note.hpp"

TEST(Person_GenderToString, Success)
{
    EXPECT_EQ("uninitialized", common::to_string(common::Gender::Uninitialized));
    EXPECT_EQ("male", common::to_string(common::Gender::Male));
    EXPECT_EQ("female", common::to_string(common::Gender::Female));
    EXPECT_EQ("invalid", common::to_string(common::Gender::Invalid));
    EXPECT_EQ("unknown", common::to_string(common::Gender::Unknown));
}


#include "common/data_table.hpp"

#include "test/tools/assertions.hpp"
#include "test/tools/gtest.hpp"

//  The extract_person_gender function tests
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

namespace test::suite_extract_person_gender
{

struct Param
{
    const char* case_name;
    const common::data_row input_row;
    const std::string input_binding_name;

    const common::Gender expected_gender;
    const std::vector<tools::ComparableNote> expected_notes;
};

class DataTable_ExtractPersonGender_ValidInput : public ::testing::TestWithParam<Param> {};

TEST_P(DataTable_ExtractPersonGender_ValidInput, NormalSuccessCases)
{
    const Param& param = GetParam();

    std::vector<common::Note> actual_notes;

    const common::Gender actual_gender = common::extract_person_gender(
        param.input_row, param.input_binding_name, actual_notes);

    tools::ComparableNote note{
            .type=common::Note::Type::Info,
            .id=std::string(common::k_unspecified_gender_note_id),
            .vars={},
            .diagnostic_text="Unspecified gender"
    };

    EXPECT_EQ(actual_gender, param.expected_gender);
    EXPECT_EQ(tools::to_comparable(actual_notes), param.expected_notes);
}

const std::vector<Param> g_normal_success_cases_params{
    {
        .case_name="EmptyRow",
        .input_row={},
        .input_binding_name="gender",
        .expected_gender=common::Gender::Unknown,
        .expected_notes={
            {
                .type=common::Note::Type::Info,
                .id=std::string(common::k_unspecified_gender_note_id),
                .vars={},
                .diagnostic_text="Unspecified gender"
            }
        }
    },
    {
        .case_name="MissingBinding",
        .input_row={
            {"az2li5ie", "KZ96HYFK317EAP0R"},
            {"ahj4eine", "47698"}
        },
        .input_binding_name="gender",
        .expected_gender=common::Gender::Unknown,
        .expected_notes={
            {
                .type=common::Note::Type::Info,
                .id=std::string(common::k_unspecified_gender_note_id),
                .vars={},
                .diagnostic_text="Unspecified gender"
            }
        },
    },
    {
        .case_name="Female",
        .input_row={
            {"person_uri", "http://data.example/33320"},
            {"person_gender", common::k_gender_uri_female.data()},
            {"uof1ig7o", "0x6d2a"}
        },
        .input_binding_name="person_gender",
        .expected_gender=common::Gender::Female,
        .expected_notes={}
    },
    {
        .case_name="Male",
        .input_row={
            {"gender_type", common::k_gender_uri_male.data()}
        },
        .input_binding_name="gender_type",
        .expected_gender=common::Gender::Male,
        .expected_notes={}
    },
    {
        .case_name="Invalid",
        .input_row={
            {"lei1aeyo", "aizanai6"}
        },
        .input_binding_name="lei1aeyo",
        .expected_gender=common::Gender::Invalid,
        .expected_notes={
            {
                .type=common::Note::Type::Error,
                .id=std::string(common::k_invalid_gender_note_id),
                .vars={},
                .diagnostic_text="Invalid gender value"
            }
        }
    }
};

INSTANTIATE_TEST_SUITE_P(
    ,
    DataTable_ExtractPersonGender_ValidInput,
    ::testing::ValuesIn(g_normal_success_cases_params),
    tools::ParamNameGen<Param>);

} // namespace test::suite_extract_person_gender
