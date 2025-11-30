#include <list>

#include <gtest/gtest.h>

#include "common/contract.hpp"
#include "common/person.hpp"
#include "common/resource.hpp"

#include "test/tools/assertions.hpp"

//  The ensure_items_not_null function tests
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

namespace test::suite_ensure_items_not_null
{

struct Param
{
    const char* case_name;
    std::vector<std::shared_ptr<common::Resource>> resources;
};

std::string ParamNameGen(const ::testing::TestParamInfo<Param>& info)
{
    return { info.param.case_name };
}

class Contract_EnsureItemsNotNull : public ::testing::TestWithParam<Param> {};

/** Check if the ensure_items_not_null function raises an input contract error exception when
 *   the input sequence  one or more null pointers. */
TEST_P(Contract_EnsureItemsNotNull, NullResourceInputContractViolation)
{
    const Param& param = GetParam();

    EXPECT_THROW_WITH_CODE(
        common::ensure_items_not_null(param.resources),
        common::common_exception, common::common_exception::error_code::input_contract_error);
}

const std::vector<Param> g_null_resource_cases_params{
    {
        .case_name="OneNullPtr",
        .resources={
            nullptr
        }
    },
    {
        .case_name="OneNullPtrTwoNonNullPtrs",
        .resources={
            std::make_shared<common::Resource>("http://example.com/R1"),
            nullptr,
            std::make_shared<common::Resource>("http://example.com/R2")
        }
    },
    {
        .case_name="TwoNullPtrsTwoNonNullPtrs",
        .resources={
            nullptr,
            std::make_shared<common::Resource>("http://example.com/R1"),
            std::make_shared<common::Resource>("http://example.com/R2"),
            nullptr
        }
    }
};

INSTANTIATE_TEST_SUITE_P(
    ,
    Contract_EnsureItemsNotNull,
    ::testing::ValuesIn(g_null_resource_cases_params),
    ParamNameGen);

TEST_F(Contract_EnsureItemsNotNull, PersonSetSuccessCase)
{
    std::set<std::shared_ptr<common::Person>> person_set = {
        std::make_shared<common::Person>("http://example.com/P1"),
        nullptr,
        std::make_shared<common::Person>("http://example.com/P3")
    };

    EXPECT_THROW_WITH_CODE(
        common::ensure_items_not_null(person_set),
        common::common_exception, common::common_exception::error_code::input_contract_error);
}

TEST_F(Contract_EnsureItemsNotNull, IntListSuccessCase)
{
    std::list<std::shared_ptr<int>> int_list = {
        std::make_shared<int>(1),
        std::make_shared<int>(2),
        std::make_shared<int>(3),
        nullptr
    };

    EXPECT_THROW_WITH_CODE(
        common::ensure_items_not_null(int_list),
        common::common_exception, common::common_exception::error_code::input_contract_error);
}

} // namespace test::suite_ensure_items_not_null
