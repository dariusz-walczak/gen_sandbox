#include <gtest/gtest.h>

#include "common/common_exception.hpp"
#include "common/data_table.hpp"

#include "test/tools/assertions.hpp"
#include "test/tools/gtest.hpp"

//  The extract_resource_uri_seq function tests
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

namespace test::suite_extract_resource_uri_seq
{

struct Param
{
    const char* case_name;
    const common::data_table input_table;
    const std::string input_binding_name;

    const std::vector<std::string> expected_uri_seq;
};

class DataTable_ExtractResourceUriSeq_ValidInput : public ::testing::TestWithParam<Param> {};

TEST_P(DataTable_ExtractResourceUriSeq_ValidInput, NormalSuccessCases)
{
    const Param& param = GetParam();

    const std::vector<std::string> actual_uri_seq =
        common::extract_resource_uri_seq(param.input_table, param.input_binding_name);

    EXPECT_EQ(actual_uri_seq, param.expected_uri_seq);
}

const std::vector<Param> g_normal_success_cases_params{
    {
        .case_name="EmptyInput",
        .input_table={},
        .input_binding_name="uri",
        .expected_uri_seq={}
    },
    {
        .case_name="OneRowOneColumn",
        .input_table={
            common::data_row{
                {"uri", "http://example.com/ID1"}
            }
        },
        .input_binding_name="uri",
        .expected_uri_seq={"http://example.com/ID1"}
    },
    {
        .case_name="OneRowThreeColumns",
        .input_table={
            common::data_row{
                {"field1", "bailu7miangu"},
                {"some_uri", "http://domain.example/x805h45r"},
                {"field2", "jein5mieshoo"}
            }
        },
        .input_binding_name="some_uri",
        .expected_uri_seq={"http://domain.example/x805h45r"}
    },
    {
        .case_name="ThreeRowsTwoColumns",
        .input_table={
            common::data_row{
                {"field1", "aaa"},
                {"field2", "http://domain.invalid/45764"}
            },
            common::data_row{
                {"field2", "http://domain.invalid/45179"},
                {"field1", "0xdec12089"}
            },
            common::data_row{
                {"field1", "0xd754727"},
                {"field2", "http://domain.invalid/23584"}
            }
        },
        .input_binding_name="field2",
        .expected_uri_seq={
            "http://domain.invalid/45764",
            "http://domain.invalid/45179",
            "http://domain.invalid/23584"
        }
    },
    {
        .case_name="UriDuplicateRow",
        .input_table={
            common::data_row{
                {"data", "C85FWLCK"},
                {"id", "http://example.net/path/A"}
            },
            common::data_row{
                {"data", "PEAXOGX1"},
                {"id", "http://example.net/path/B"}
            },
            common::data_row{
                {"data", "7FTXFDWU"},
                {"id", "http://example.net/path/A"}
            }
        },
        .input_binding_name="id",
        .expected_uri_seq={
            "http://example.net/path/A",
            "http://example.net/path/B",
            "http://example.net/path/A"
        }
    },
    {
        .case_name="FullDuplicateRow",
        .input_table={
            common::data_row{
                {"data", "C85FWLCK"},
                {"id", "http://example.net/path/A"}
            },
            common::data_row{
                {"data", "PEAXOGX1"},
                {"id", "http://example.net/path/B"}
            },
            common::data_row{
                {"data", "PEAXOGX1"},
                {"id", "http://example.net/path/B"}
            },
            common::data_row{
                {"data", "7FTXFDWU"},
                {"id", "http://example.net/path/C"}
            }
        },
        .input_binding_name="id",
        .expected_uri_seq={
            "http://example.net/path/A",
            "http://example.net/path/B",
            "http://example.net/path/B",
            "http://example.net/path/C"
        }
    },
    {
        .case_name="MultipleUriColumns",
        .input_table={
            common::data_row{
                {"uri1", "http://base.example.org/A"},
                {"uri2", "http://ext.example.org/1"},
                {"wff7", "0"}
            },
            common::data_row{
                {"uri1", "http://base.example.org/C"},
                {"wff7", "0"},
                {"uri2", "http://ext.example.org/3"}
            },
            common::data_row{
                {"wff7", "1"},
                {"uri1", "http://base.example.org/B"},
                {"uri2", "http://ext.example.org/2"}
            }
        },
        .input_binding_name="uri2",
        .expected_uri_seq={
            "http://ext.example.org/1",
            "http://ext.example.org/3",
            "http://ext.example.org/2"
        }
    }
};

INSTANTIATE_TEST_SUITE_P(
    ,
    DataTable_ExtractResourceUriSeq_ValidInput,
    ::testing::ValuesIn(g_normal_success_cases_params),
    tools::ParamNameGen<Param>);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

class DataTable_ExtractResourceUriSeq_MissingBinding : public ::testing::TestWithParam<Param> {};

TEST_P(DataTable_ExtractResourceUriSeq_MissingBinding, BindingNotFoundErrorCase)
{
    const Param& param = GetParam();

    EXPECT_THROW_WITH_CODE(
        common::extract_resource_uri_seq(param.input_table, param.input_binding_name),
        common::common_exception, common::common_exception::error_code::binding_not_found);
}

const std::vector<Param> g_missing_binding_error_cases_params{
    {
        .case_name="OneRowOneColumn",
        .input_table={
            common::data_row{
                {"ly9v", "http://example.com/ID1"}
            }
        },
        .input_binding_name="uri",
        .expected_uri_seq={}
    },
    {
        .case_name="OneRowThreeColumns",
        .input_table={
            common::data_row{
                {"field1", "bailu7miangu"},
                {"field3", "http://domain.example/x805h45r"},
                {"field2", "jein5mieshoo"}
            }
        },
        .input_binding_name="some_uri",
        .expected_uri_seq={}
    },
    {
        .case_name="SingleBindingNameMismatch",
        .input_table={
            common::data_row{
                {"field1", "aaa"},
                {"uri", "http://domain.invalid/45764"}
            },
            common::data_row{
                {"uri", "http://domain.invalid/45179"},
                {"field1", "0xdec12089"}
            },
            common::data_row{
                {"field1", "0xd754727"},
                {"field_name_mismatch", "http://domain.invalid/23584"}
            }
        },
        .input_binding_name="uri",
        .expected_uri_seq={}
    },
    {
        .case_name="RemovedBinding",
        .input_table={
            common::data_row{
                {"field1", "aaa"},
                {"uri", "http://domain.invalid/45764"}
            },
            common::data_row{
                {"field1", "0xdec12089"}
            },
            common::data_row{
                {"field1", "0xd754727"},
                {"uri", "http://domain.invalid/23584"}
            }
        },
        .input_binding_name="uri",
        .expected_uri_seq={}
    }
};

INSTANTIATE_TEST_SUITE_P(
    ,
    DataTable_ExtractResourceUriSeq_MissingBinding,
    ::testing::ValuesIn(g_missing_binding_error_cases_params),
    tools::ParamNameGen<Param>);

} // namespace test::suite_extract_resource_uri_seq

//  The get_boolean_value_req function tests
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

namespace test::suite_get_boolean_value_req
{

struct ValidInputParam
{
    const char* case_name;
    const common::data_row input_data_row;
    const std::string input_binding_name;

    const bool expected_value;
};

class DataTable_GetBooleanValueReq_ValidInput
    : public ::testing::TestWithParam<ValidInputParam> {};

TEST_P(DataTable_GetBooleanValueReq_ValidInput, NormalSuccessCases)
{
    const ValidInputParam& param = GetParam();    

    const auto actual_value =
        common::get_boolean_value_req(param.input_data_row, param.input_binding_name);

    EXPECT_EQ(actual_value, param.expected_value);
}

const std::vector<ValidInputParam> g_normal_success_cases_params{
    {
        .case_name="True",
        .input_data_row={{"flag1", "true"}, {"flag2", "false"}, {"name", "ums7"}},
        .input_binding_name="flag1",
        .expected_value=true
    },
    {
        .case_name="False",
        .input_data_row={{"flag1", "false"}, {"flag2", "true"}, {"name", "y6gg"}},
        .input_binding_name="flag1",
        .expected_value=false
    }
};

INSTANTIATE_TEST_SUITE_P(
    ,
    DataTable_GetBooleanValueReq_ValidInput,
    ::testing::ValuesIn(g_normal_success_cases_params),
    tools::ParamNameGen<ValidInputParam>);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

struct InvalidInputParam
{
    const char* case_name;
    const common::data_row input_data_row;
    const std::string input_binding_name;

    const common::common_exception::error_code expected_error_code;
};

class DataTable_GetBooleanValueReq_InvalidInput
    : public ::testing::TestWithParam<InvalidInputParam> {};

TEST_P(DataTable_GetBooleanValueReq_InvalidInput, ErrorCases)
{
    const InvalidInputParam& param = GetParam();    

    EXPECT_THROW_WITH_CODE(
        common::get_boolean_value_req(param.input_data_row, param.input_binding_name),
        common::common_exception, param.expected_error_code);
}

const std::vector<InvalidInputParam> g_error_cases_params{
    {
        .case_name="BindingNotFound_EmptyInput",
        .input_data_row={},
        .input_binding_name="some_flag",
        .expected_error_code=common::common_exception::error_code::binding_not_found
    },
    {
        .case_name="BindingNotFound_NameMismatch",
        .input_data_row={{"iu5l", "j98p"}, {"flag", "false"}, {"ohd2", "3h46"}},
        .input_binding_name="mismatched_flag_name",
        .expected_error_code=common::common_exception::error_code::binding_not_found
    },
    {
        .case_name="InputContractError_UppercaseTrue",
        .input_data_row={{"flag", "TRUE"}, {"value", "0x75"}},
        .input_binding_name="flag",
        .expected_error_code=common::common_exception::error_code::input_contract_error
    },
    {
        .case_name="InputContractError_CapitalizedFalse",
        .input_data_row={{"flag", "False"}, {"value", "0x61"}},
        .input_binding_name="flag",
        .expected_error_code=common::common_exception::error_code::input_contract_error
    },
    {
        .case_name="InputContractError_Zero",
        .input_data_row={{"flag", "0"}, {"value", "0xC1"}},
        .input_binding_name="flag",
        .expected_error_code=common::common_exception::error_code::input_contract_error
    },
    {
        .case_name="InputContractError_RandomString",
        .input_data_row={{"flag", "v36zh4gh2f5m"}, {"value", "0xEC"}},
        .input_binding_name="flag",
        .expected_error_code=common::common_exception::error_code::input_contract_error
    }
};

INSTANTIATE_TEST_SUITE_P(
    ,
    DataTable_GetBooleanValueReq_InvalidInput,
    ::testing::ValuesIn(g_error_cases_params),
    tools::ParamNameGen<InvalidInputParam>);

} // namespace test::suite_get_boolean_value_req
