#include <gtest/gtest.h>

#include "common/common_exception.hpp"
#include "common/person_utils.hpp"

#include "test/tools/application.hpp"
#include "test/tools/assertions.hpp"
#include "test/tools/gtest.hpp"
#include "test/tools/redland.hpp"

//  Tests of the ask_person_gender_unknown function:
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

namespace test::suite_ask_person_state
{

struct Param
{
    const char* case_name;
    const char* data_file;
    const char* proband_uri;

    const bool expected_gender_unknown_flag;
    const bool expected_gender_invalid_flag;
    const bool expected_gender_valid_flag;
};

inline std::ostream& operator<<(std::ostream& os, const Param& param)
{
    return os << "Param{\n" << std::boolalpha <<
        "    case_name='" << param.case_name << "'\n" <<
        "    data_file='" << param.data_file << "'\n" <<
        "    proband_uri='" << param.proband_uri << "'\n" <<
        "    expected_gender_unknown_flag=" <<
        param.expected_gender_unknown_flag << "\n" <<
        "    expected_gender_invalid_flag=" <<
        param.expected_gender_invalid_flag << "\n" <<
        "    expected_gender_valid_flag=" <<
        param.expected_gender_valid_flag << "\n" <<
        "}";
}

class PersonUtils_AskPersonGenderState : public ::testing::TestWithParam<Param> {};

TEST_P(PersonUtils_AskPersonGenderState, NormalSuccessCases)
{
    const Param& param = GetParam();
    tools::scoped_redland_ctx ctx = tools::initialize_redland_ctx();
    tools::load_rdf(ctx->world, ctx->model, tools::get_program_path() / param.data_file);

    const auto person = std::make_shared<common::Person>(param.proband_uri);

    const bool actual_gender_unknown_flag =
        common::ask_person_gender_unknown(person.get(), ctx->world, ctx->model);
    const bool actual_gender_invalid_flag =
        common::ask_person_gender_invalid(person.get(), ctx->world, ctx->model);

    EXPECT_EQ(actual_gender_unknown_flag, param.expected_gender_unknown_flag);
    EXPECT_EQ(actual_gender_invalid_flag, param.expected_gender_invalid_flag);
}

const std::vector<Param> g_normal_success_cases_params{
    {
        .case_name="UnknownPerson",
        .data_file="data/person_utils/model-01_person-gender-state-variants.ttl",
        .proband_uri="http://example.org/V101P1",
        .expected_gender_unknown_flag=true,
        .expected_gender_invalid_flag=false,
        .expected_gender_valid_flag=false
    },
    {
        .case_name="Unknown1",
        .data_file="data/person_utils/model-01_person-gender-state-variants.ttl",
        .proband_uri="http://example.org/V102P1",
        .expected_gender_unknown_flag=true,
        .expected_gender_invalid_flag=false,
        .expected_gender_valid_flag=false
    },
    {
        .case_name="Unknown2",
        .data_file="data/person_utils/model-01_person-gender-state-variants.ttl",
        .proband_uri="http://example.org/V103P1",
        .expected_gender_unknown_flag=true,
        .expected_gender_invalid_flag=false,
        .expected_gender_valid_flag=false
    },
    {
        .case_name="Unknown3",
        .data_file="data/person_utils/model-01_person-gender-state-variants.ttl",
        .proband_uri="http://example.org/V104P1",
        .expected_gender_unknown_flag=true,
        .expected_gender_invalid_flag=false,
        .expected_gender_valid_flag=false
    },
    {
        .case_name="Unknown4",
        .data_file="data/person_utils/model-01_person-gender-state-variants.ttl",
        .proband_uri="http://example.org/V105P1",
        .expected_gender_unknown_flag=true,
        .expected_gender_invalid_flag=false,
        .expected_gender_valid_flag=false
    },
    {
        .case_name="Invalid",
        .data_file="data/person_utils/model-01_person-gender-state-variants.ttl",
        .proband_uri="http://example.org/V106P1",
        .expected_gender_unknown_flag=false,
        .expected_gender_invalid_flag=true,
        .expected_gender_valid_flag=false
    },
    {
        .case_name="ValidMale",
        .data_file="data/person_utils/model-01_person-gender-state-variants.ttl",
        .proband_uri="http://example.org/V107P1",
        .expected_gender_unknown_flag=false,
        .expected_gender_invalid_flag=false,
        .expected_gender_valid_flag=true
    },
    {
        .case_name="ValidFemale",
        .data_file="data/person_utils/model-01_person-gender-state-variants.ttl",
        .proband_uri="http://example.org/V108P1",
        .expected_gender_unknown_flag=false,
        .expected_gender_invalid_flag=false,
        .expected_gender_valid_flag=true
    }
};

INSTANTIATE_TEST_SUITE_P(
    ,
    PersonUtils_AskPersonGenderState,
    ::testing::ValuesIn(g_normal_success_cases_params),
    tools::ParamNameGen<Param>);

/** @brief Check if functions from the ask person state family signal input contract error when any
 *      of their pointer arguments are null. */
TEST_F(PersonUtils_AskPersonGenderState, InputContractViolations)
{
    tools::scoped_redland_ctx ctx = tools::initialize_redland_ctx();
    const auto person = std::make_shared<common::Person>("http://example.org/someone");

    EXPECT_THROW_WITH_CODE(
        common::ask_person_gender_unknown(nullptr, ctx->world, ctx->model),
        common::common_exception, common::common_exception::error_code::input_contract_error);
    EXPECT_THROW_WITH_CODE(
        common::ask_person_gender_unknown(person.get(), nullptr, ctx->model),
        common::common_exception, common::common_exception::error_code::input_contract_error);
    EXPECT_THROW_WITH_CODE(
        common::ask_person_gender_unknown(person.get(), ctx->world, nullptr),
        common::common_exception, common::common_exception::error_code::input_contract_error);
}

} // namespace test::suite_ask_person_state
