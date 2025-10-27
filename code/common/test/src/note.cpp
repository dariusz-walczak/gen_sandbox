#include <gtest/gtest.h>

#include "test/tools/person.hpp"

#include "common/note.hpp"

TEST(Note_TypeToString, Success)
{
    EXPECT_EQ("uninitialized", common::to_string(common::Note::Type::Uninitialized));
    EXPECT_EQ("info", common::to_string(common::Note::Type::Info));
    EXPECT_EQ("warning", common::to_string(common::Note::Type::Warning));
    EXPECT_EQ("error", common::to_string(common::Note::Type::Error));
}

//  The retrieve_person_father and retrieve_person_mother functions tests
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

namespace suite1
{

struct Param
{
    const char* case_name;
    common::Note input_note;
    nlohmann::json expected_json;
};

class Note_NoteToJson : public ::testing::TestWithParam<Param> {};

TEST_P(Note_NoteToJson, NormalSuccessCases)
{
    const Param& param = GetParam();

    const nlohmann::json actual_json = note_to_json(param.input_note);

    EXPECT_EQ(param.expected_json, actual_json);
};

const std::vector<Param> g_params {
    {
        "StaticInfoNote",
        common::Note{common::Note::Type::Info, "NOTE1", {}, "Static Note #1: Info Type"},
        {
            {"type", "info"},
            {"diag", "Static Note #1: Info Type"},
            {"id", "NOTE1"},
            {"vars", nlohmann::json::object()} // to achieve {} instead of null
        }
    },
    {
        "StaticWarningNote",
        common::Note{common::Note::Type::Warning, "NOTE2", {}, "Static Note #2: Warning Type"},
        {
            {"type", "warning"},
            {"diag", "Static Note #2: Warning Type"},
            {"id", "NOTE2"},
            {"vars", nlohmann::json::object()} // to achieve {} instead of null
        }
    },
    {
        "StaticErrorNote",
        common::Note{common::Note::Type::Error, "NOTE3", {}, "Static Note #3: Error Type"},
        {
            {"type", "error"},
            {"diag", "Static Note #3: Error Type"},
            {"id", "NOTE3"},
            {"vars", nlohmann::json::object()} // to achieve {} instead of null
        }
    },
    {
        "DynamicInt",
        common::Note{
            common::Note::Type::Info, "NOTE4", {{"var1", 100}}, "Dynamic Note #4: Single Int"},
        {
            {"type", "info"},
            {"diag", "Dynamic Note #4: Single Int"},
            {"id", "NOTE4"},
            {"vars", {{"var1", 100}}}
        }
    },
    {
        "DynamicStr",
        common::Note{
            common::Note::Type::Info, "NOTE5",
            {{"var1", "value"}}, "Dynamic Note #5: Single String"},
        {
            {"type", "info"},
            {"diag", "Dynamic Note #5: Single String"},
            {"id", "NOTE5"},
            {"vars", {{"var1", "value"}}}
        }
    },
    {
        "DynamicRes",
        common::Note{
            common::Note::Type::Info, "NOTE6",
            {{"var1", std::make_shared<common::Resource>("http://example.com/R1")}},
            "Dynamic Note #6: Single Resource"},
        {
            {"type", "info"},
            {"diag", "Dynamic Note #6: Single Resource"},
            {"id", "NOTE6"},
            {"vars", {{"var1", {{"unique_path", "example.com/R1"}, {"caption", ""}}}}}
        }
    },
    {
        "DynamicResIntIntStrResStr",
        common::Note{
            common::Note::Type::Info, "NOTE7",
            {
                {"var1", std::make_shared<common::Resource>("http://example.com/R1")},
                {"var2", 123},
                {"var3", 1337},
                {"var4", "some text"},
                {"var5", std::make_shared<common::Resource>("http://example.com/R2")},
                {"var6", "another text"}
            },
            "Dynamic Note #7: Res Int Int Str Res Str"},
        {
            {"type", "info"},
            {"diag", "Dynamic Note #7: Res Int Int Str Res Str"},
            {"id", "NOTE7"},
            {
                "vars",
                {
                    {"var1", {{"unique_path", "example.com/R1"}, {"caption", ""}}},
                    {"var2", 123},
                    {"var3", 1337},
                    {"var4", "some text"},
                    {"var5", {{"unique_path", "example.com/R2"}, {"caption", ""}}},
                    {"var6", "another text"}
                }
            }
        }
    },
    {
        "DynamicPer",
        common::Note{
            common::Note::Type::Info, "NOTE8",
            {{"var1", test::tools::construct_named_person_ptr(
                        "http://example.com/P1", "Ignacy", "Daszyński")}},
            "Dynamic Note #8: Single Person"},
        {
            {"type", "info"},
            {"diag", "Dynamic Note #8: Single Person"},
            {"id", "NOTE8"},
            {"vars", {{"var1", {{"unique_path", "example.com/P1"},
                                {"caption", "Daszyński, Ignacy"}}}}}
        }
    }
};

std::string ParamNameGen(const ::testing::TestParamInfo<Param>& info)
{
    return { info.param.case_name };
}


INSTANTIATE_TEST_SUITE_P(
    ,
    Note_NoteToJson,
    ::testing::ValuesIn(g_params),
    ParamNameGen);

} // namespace suite1
