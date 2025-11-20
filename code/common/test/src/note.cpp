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
    const char* expected_json;
};

class Note_NoteToJson : public ::testing::TestWithParam<Param> {};

TEST_P(Note_NoteToJson, NormalSuccessCases)
{
    const Param& param = GetParam();

    const nlohmann::json actual_json = note_to_json(param.input_note);
    const nlohmann::json expected_json = nlohmann::json::parse(param.expected_json);

    EXPECT_EQ(expected_json, actual_json);
};

const std::vector<Param> g_params {
    {
        "StaticInfoNote",
        common::Note{common::Note::Type::Info, "NOTE1", {}, "Static Note #1: Info Type"},
        R"({
            "type": "info",
            "diag": "Static Note #1: Info Type",
            "id": "NOTE1",
            "vars": {}
        })"
    },
    {
        "StaticWarningNote",
        common::Note{common::Note::Type::Warning, "NOTE2", {}, "Static Note #2: Warning Type"},
        R"({
            "type": "warning",
            "diag": "Static Note #2: Warning Type",
            "id": "NOTE2",
            "vars": {}
        })"
    },
    {
        "StaticErrorNote",
        common::Note{common::Note::Type::Error, "NOTE3", {}, "Static Note #3: Error Type"},
        R"({
            "type": "error",
            "diag": "Static Note #3: Error Type",
            "id": "NOTE3",
            "vars": {}
        })"
    },
    {
        "DynamicInt",
        common::Note{
            common::Note::Type::Info, "NOTE4", {{"var1", 100}}, "Dynamic Note #4: Single Int"},
        R"({
            "type": "info",
            "diag": "Dynamic Note #4: Single Int",
            "id": "NOTE4",
            "vars": {
                "var1": {
                    "name": "var1",
                    "type": "integer",
                    "value": 100
                }
            }
        })"
    },
    {
        "DynamicStr",
        common::Note{
            common::Note::Type::Info, "NOTE5",
            {{"var1", "value"}}, "Dynamic Note #5: Single String"},
        R"({
            "type": "info",
            "diag": "Dynamic Note #5: Single String",
            "id": "NOTE5",
            "vars": {
                "var1": {
                    "name": "var1",
                    "type": "string",
                    "value": "value"
                }
            }
        })"
    },
    {
        "DynamicRes",
        common::Note{
            common::Note::Type::Info, "NOTE6",
            {{"var1", std::make_shared<common::Resource>("http://example.com/R1")}},
            "Dynamic Note #6: Single Resource"},
        R"({
            "type": "info",
            "diag": "Dynamic Note #6: Single Resource",
            "id": "NOTE6",
            "vars": {
                "var1": {
                    "name": "var1",
                    "type": "resource",
                    "value": {"unique_path": "example.com/R1", "caption": ""}
                }
            }
        })"
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
        R"({
            "type": "info",
            "diag": "Dynamic Note #7: Res Int Int Str Res Str",
            "id": "NOTE7",
            "vars": {
                "var1": {
                    "name": "var1",
                    "type": "resource",
                    "value": {"unique_path": "example.com/R1", "caption": ""}
                },
                "var2": {
                    "name": "var2",
                    "type": "integer",
                    "value": 123
                },
                "var3": {
                    "name": "var3",
                    "type": "integer",
                    "value": 1337
                },
                "var4": {
                    "name": "var4",
                    "type": "string",
                    "value": "some text"
                },
                "var5": {
                    "name": "var5",
                    "type": "resource",
                    "value": {"unique_path": "example.com/R2", "caption": ""}
                },
                "var6": {
                    "name": "var6",
                    "type": "string",
                    "value": "another text"
                }
            }
        })"
    },
    {
        "DynamicPer",
        common::Note{
            common::Note::Type::Info, "NOTE8",
            {{"var1", test::tools::construct_named_person_ptr(
                        "http://example.com/P1", "Ignacy", "Daszyński")}},
            "Dynamic Note #8: Single Person"},
        R"({
            "type": "info",
            "diag": "Dynamic Note #8: Single Person",
            "id": "NOTE8",
            "vars": {
                "var1": {
                    "name": "var1",
                    "type": "resource",
                    "value": {"unique_path": "example.com/P1", "caption": "Daszyński, Ignacy"}
                }
            }
        })"
    },
    {
        "DynamicIntList",
        common::Note{
            common::Note::Type::Info, "NOTE9",
            {{"var1", std::vector<common::Variable>{{"", 1}, {"", 2}, {"", 3}}}},
            "Dynamic Note #9: Int List Variable"},
        R"({
            "type": "info",
            "diag": "Dynamic Note #9: Int List Variable",
            "id": "NOTE9",
            "vars": {
                "var1": {
                    "name": "var1",
                    "type": "sequence",
                    "value": [
                        {
                            "name": "",
                            "type": "integer",
                            "value": 1
                        }, {
                            "name": "",
                            "type": "integer",
                            "value": 2
                        }, {
                            "name": "",
                            "type": "integer",
                            "value": 3
                        }
                    ]
                }
            }
        })"
    },
    {
        "DynamicMixedList",
        common::Note{
            common::Note::Type::Info, "NOTE10",
            {{"var1", std::vector<common::Variable>{
                        {"var1a", 1},
                        {"var1b", "test"},
                        {"var1c", std::make_shared<common::Resource>("http://example.com/R1")}}}},
            "Dynamic Note #10: Mixed List Variable"},
        R"({
            "type": "info",
            "diag": "Dynamic Note #10: Mixed List Variable",
            "id": "NOTE10",
            "vars": {
                "var1": {
                    "name": "var1",
                    "type": "sequence",
                    "value": [
                        {
                            "name": "var1a",
                            "type": "integer",
                            "value": 1
                        }, {
                            "name": "var1b",
                            "type": "string",
                            "value": "test"
                        }, {
                            "name": "var1c",
                            "type": "resource",
                            "value": {"unique_path": "example.com/R1", "caption": ""}
                        }
                    ]
                }
            }
        })"
    },
    {
        "DynamicThreeLevelTree",
        common::Note{
            common::Note::Type::Info, "NOTE11",
            {
                {"var1", std::vector<common::Variable>{
                        {"var2a", 65455073},
                        {"var2b", std::vector<common::Variable>{
                                {"var2b1", std::vector<common::Variable>{}},
                                {"var2b2", "ohn7phuphagh"},
                                {"var2b3", std::vector<common::Variable>{
                                        {"var2b3a", "ahthoor4zai2"},
                                        {"var2b3b", "aizoofahpu8a"}}}}}}},
                {"var2", 47689124},
                {"var3", -4748167}},
            "Dynamic Note #11: Three Level Tree"},
        R"({
            "type": "info",
            "diag": "Dynamic Note #11: Three Level Tree",
            "id": "NOTE11",
            "vars": {
                "var1": {
                    "name": "var1",
                    "type": "sequence",
                    "value": [
                        {
                            "name": "var2a",
                            "type": "integer",
                            "value": 65455073
                        }, {
                            "name": "var2b",
                            "type": "sequence",
                            "value": [
                                {
                                    "name": "var2b1",
                                    "type": "sequence",
                                    "value": []
                                }, {
                                    "name": "var2b2",
                                    "type": "string",
                                    "value": "ohn7phuphagh"
                                }, {
                                    "name": "var2b3",
                                    "type": "sequence",
                                    "value": [
                                        {
                                            "name": "var2b3a",
                                            "type": "string",
                                            "value": "ahthoor4zai2"
                                        }, {
                                            "name": "var2b3b",
                                            "type": "string",
                                            "value": "aizoofahpu8a"
                                        }
                                    ]
                                }
                            ]
                        }
                    ]
                },
                "var2": {
                    "name": "var2",
                    "type": "integer",
                    "value": 47689124
                },
                "var3": {
                    "name": "var3",
                    "type": "integer",
                    "value": -4748167
                }
            }
        })"
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
