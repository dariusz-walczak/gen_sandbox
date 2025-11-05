#include <algorithm>
#include <memory>
#include <variant>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "test/tools/application.hpp"
#include "test/tools/assertions.hpp"
#include "test/tools/error.hpp"
#include "test/tools/matchers.hpp"
#include "test/tools/note.hpp"
#include "test/tools/printers.hpp"
#include "test/tools/redland.hpp"
#include "test/tools/resource.hpp"

#include "common/comparators.hpp"
#include "person/error.hpp"
#include "person/queries/details.hpp"

namespace test
{

namespace
{

struct ComparablePartnerRelation
{
    common::Person partner;
    bool is_inferred;

    bool operator==(const ComparablePartnerRelation& other) const {
        return ((partner == other.partner) && (is_inferred == other.is_inferred));
    }

    friend void PrintTo(const ComparablePartnerRelation& rel, std::ostream* os) {
        *os << "ComparablePartnerRelation{partner: ";
        PrintTo(rel.partner, os);
        *os << ", is_inferred=" << std::boolalpha << rel.is_inferred << "}";
    };
};

std::vector<ComparablePartnerRelation> adapt(
    const std::vector<common::Person::PartnerRelation>& partners)
{
    std::vector<ComparablePartnerRelation> output;
    output.reserve(partners.size());
    std::transform(
        partners.cbegin(), partners.cend(), std::back_inserter(output),
        [](const auto& rel) { return ComparablePartnerRelation{*rel.partner, rel.is_inferred}; });
    return output;
}

} // anonymous namespace

//  The retrieve_person_partners function tests
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

namespace
{

struct Param
{
    const char* case_name;
    const char* data_file;
    const char* proband_uri;

    std::vector<ComparablePartnerRelation> expected_partners;
    std::vector<tools::ComparableNote> expected_notes;
};

class DetailsQueries_RetrievePersonPartners : public ::testing::TestWithParam<Param> {};

TEST_P(DetailsQueries_RetrievePersonPartners, NormalSuccessCases)
{
    const Param& param = GetParam();
    tools::scoped_redland_ctx ctx = tools::initialize_redland_ctx();
    tools::load_rdf(ctx->world, ctx->model, tools::get_program_path() / param.data_file);

    const auto proband = std::make_shared<common::Person>(param.proband_uri);
    std::vector<common::Note> actual_notes;

    const auto& actual_partners = adapt(
        person::retrieve_person_partners(
            proband.get(), ctx->world, ctx->model, actual_notes));

    EXPECT_THAT(param.expected_partners, actual_partners);
    EXPECT_EQ(param.expected_notes, tools::to_comparable(actual_notes));
};

const std::vector<Param> g_params {
    {
        "NoPartner",
        "data/deps_queries/retrieve_person_partners/normal_success_cases/model-00_no-partner.ttl",
        "http://example.org/P1",
        {
        },
        {
        }
    },
    {
        "SinglePartner",
        "data/deps_queries/retrieve_person_partners/normal_success_cases/model-01_one-partner.ttl",
        "http://example.org/P1",
        {
            {common::Person("http://example.org/P2"), false}
        },
        {
        }
    },
    {
        "SinglePartnerInverted",
        "data/deps_queries/retrieve_person_partners/normal_success_cases/model-01_one-partner.ttl",
        "http://example.org/P2",
        {
            {common::Person("http://example.org/P1"), false}
        },
        {
        }
    },
    {
        "TwoPartnersP1",
        "data/deps_queries/retrieve_person_partners/normal_success_cases/"
        "model-02_two-partners.ttl",
        "http://example.org/P1",
        {
            {common::Person("http://example.org/P2"), false},
            {common::Person("http://example.org/P3"), false}
        },
        {
        }
    },
    {
        "TwoPartnersP2",
        "data/deps_queries/retrieve_person_partners/normal_success_cases/"
        "model-02_two-partners.ttl",
        "http://example.org/P2",
        {
            {common::Person("http://example.org/P1"), false}
        },
        {
        }
    },
    {
        "TwoPartnersP3",
        "data/deps_queries/retrieve_person_partners/normal_success_cases/"
        "model-02_two-partners.ttl",
        "http://example.org/P3",
        {
            {common::Person("http://example.org/P1"), false},
            {common::Person("http://example.org/P4"), false}
        },
        {
        }
    },
    {
        "ThreeGensG1P1",
        "data/deps_queries/retrieve_person_partners/normal_success_cases/"
        "model-03_three-generations.ttl",
        "http://example.org/G1P1",
        {
            {common::Person("http://example.org/G1P2"), false}
        },
        {
        }
    },
    {
        "ThreeGensG2P1",
        "data/deps_queries/retrieve_person_partners/normal_success_cases/"
        "model-03_three-generations.ttl",
        "http://example.org/G2P1",
        {
            {common::Person("http://example.org/G2P2"), false}
        },
        {
        }
    },
    {
        "ThreeGensG3P1",
        "data/deps_queries/retrieve_person_partners/normal_success_cases/"
        "model-03_three-generations.ttl",
        "http://example.org/G3P1",
        {
        },
        {
        }
    },
    {
        "OWL84_P00010",
        "data/deps_queries/retrieve_person_partners/normal_success_cases/"
        "model-04_owl-84-repro.ttl",
        "http://example.org/P00010",
        {
            {common::Person("http://example.org/P00000"), false}
        },
        {
        }
    },
    {
        "ThreeGensInferredG1P1",
        "data/deps_queries/retrieve_person_partners/normal_success_cases/"
        "model-05_three-generations-some-inferred.ttl",
        "http://example.org/G1P1",
        {
            {common::Person("http://example.org/G1P2"), true}
        },
        {
            {
                common::Note::Type::Info,
                std::string(person::k_inferred_partner_note_id),
                {{"partner", {tools::ComparableResource{"http://example.org/G1P2", ""}}}},
                "Partner inferred: http://example.org/G1P2"
            }
        }
    },
    {
        "ThreeGensInferredG1P2",
        "data/deps_queries/retrieve_person_partners/normal_success_cases/"
        "model-05_three-generations-some-inferred.ttl",
        "http://example.org/G1P2",
        {
            {common::Person("http://example.org/G1P1"), true}
        },
        {
            {
                common::Note::Type::Info,
                std::string(person::k_inferred_partner_note_id),
                {{"partner", {tools::ComparableResource{"http://example.org/G1P1", ""}}}},
                "Partner inferred: http://example.org/G1P1"
            }
        }
    },
    {
        "ThreeGensInferredG1P3",
        "data/deps_queries/retrieve_person_partners/normal_success_cases/"
        "model-05_three-generations-some-inferred.ttl",
        "http://example.org/G1P3",
        {
            {common::Person("http://example.org/G1P4"), false}
        },
        {
        }
    },
    {
        "ThreeGensInferredG2P1",
        "data/deps_queries/retrieve_person_partners/normal_success_cases/"
        "model-05_three-generations-some-inferred.ttl",
        "http://example.org/G2P1",
        {
            {common::Person("http://example.org/G2P2"), true}
        },
        {
            {
                common::Note::Type::Info,
                std::string(person::k_inferred_partner_note_id),
                {{"partner", {tools::ComparableResource{"http://example.org/G2P2", ""}}}},
                "Partner inferred: http://example.org/G2P2"
            }
        }
    },
    {
        "ThreeGensInferredG2P2",
        "data/deps_queries/retrieve_person_partners/normal_success_cases/"
        "model-05_three-generations-some-inferred.ttl",
        "http://example.org/G2P2",
        {
            {common::Person("http://example.org/G2P1"), true}
        },
        {
            {
                common::Note::Type::Info,
                std::string(person::k_inferred_partner_note_id),
                {{"partner", {tools::ComparableResource{"http://example.org/G2P1", ""}}}},
                "Partner inferred: http://example.org/G2P1"
            }
        }
    },
    {
        "ThreeGensInferredG3P1",
        "data/deps_queries/retrieve_person_partners/normal_success_cases/"
        "model-05_three-generations-some-inferred.ttl",
        "http://example.org/G3P1",
        {
        },
        {
        }
    }
};

std::string ParamNameGen(const ::testing::TestParamInfo<Param>& info)
{
    return { info.param.case_name };
}


INSTANTIATE_TEST_SUITE_P(
    ,
    DetailsQueries_RetrievePersonPartners,
    ::testing::ValuesIn(g_params),
    ParamNameGen);

// Check if the retrieve_person_partners function throws the person_exception when any of its
//  arguments is null
TEST_F(DetailsQueries_RetrievePersonPartners, InputContractViolations)
{
    tools::scoped_redland_ctx ctx = tools::initialize_redland_ctx();
    const auto person = std::make_shared<common::Person>("http://example.org/someone");
    std::vector<common::Note> notes;

    EXPECT_THROW_WITH_CODE(
        person::retrieve_person_partners(nullptr, ctx->world, ctx->model, notes),
        person::person_exception, person::person_exception::error_code::input_contract_error);

    EXPECT_THROW_WITH_CODE(
        person::retrieve_person_partners(person.get(), nullptr, ctx->model, notes),
        person::person_exception, person::person_exception::error_code::input_contract_error);

    EXPECT_THROW_WITH_CODE(
        person::retrieve_person_partners(person.get(), ctx->world, nullptr, notes),
        person::person_exception, person::person_exception::error_code::input_contract_error);
}

} // anonymous namespace

//  The retrieve_person_father and retrieve_person_mother functions tests
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

namespace suite2
{

struct Param
{
    const char* case_name;
    const char* data_file;
    const char* proband_uri;

    std::optional<common::Person> expected_father;
    std::optional<common::Person> expected_mother;
};

class DetailsQueries_RetrievePersonParents : public ::testing::TestWithParam<Param> {};

TEST_P(DetailsQueries_RetrievePersonParents, NormalSuccessCases)
{
    const Param& param = GetParam();
    tools::scoped_redland_ctx ctx = tools::initialize_redland_ctx();
    tools::load_rdf(ctx->world, ctx->model, tools::get_program_path() / param.data_file);

    const auto proband = std::make_shared<common::Person>(param.proband_uri);

    const auto& actual_father =
        person::retrieve_person_father_opt(proband.get(), ctx->world, ctx->model);
    const auto& actual_mother =
        person::retrieve_person_mother_opt(proband.get(), ctx->world, ctx->model);

    if (param.expected_father.has_value())
    {
        EXPECT_EQ(param.expected_father.value(), *actual_father);
    }
    else
    {
        EXPECT_EQ(nullptr, actual_father.get());
    }

    if (param.expected_mother.has_value())
    {
        EXPECT_EQ(param.expected_mother.value(), *actual_mother);
    }
    else
    {
        EXPECT_EQ(nullptr, actual_mother.get());
    }
};

const std::vector<Param> g_params {
    {
        "NoParents",
        "data/deps_queries/retrieve_person_parents/normal_success_cases/model-00_no-parents.ttl",
        "http://example.org/P1",
        {},
        {}
    },
    {
        "FatherOnly",
        "data/deps_queries/retrieve_person_parents/normal_success_cases/model-01_father-only.ttl",
        "http://example.org/P1",
        {common::Person("http://example.org/P2")},
        {}
    },
    {
        "MotherOnly",
        "data/deps_queries/retrieve_person_parents/normal_success_cases/model-02_mother-only.ttl",
        "http://example.org/P1",
        {},
        {common::Person("http://example.org/P3")}
    },
    {
        "BothParents",
        "data/deps_queries/retrieve_person_parents/normal_success_cases/model-03_both-parents.ttl",
        "http://example.org/P1",
        {common::Person("http://example.org/P2")},
        {common::Person("http://example.org/P3")}
    },
    {
        "TwoFamiliesTheeGenerationsF1G3P1",
        "data/deps_queries/retrieve_person_parents/normal_success_cases/"
        "model-04_two-families-three-generations.ttl",
        "http://example.org/F1G3P1",
        {common::Person("http://example.org/F1G2P1")},
        {common::Person("http://example.org/F1G2P2")}
    },
    {
        "TwoFamiliesTheeGenerationsF1G3P2",
        "data/deps_queries/retrieve_person_parents/normal_success_cases/"
        "model-04_two-families-three-generations.ttl",
        "http://example.org/F1G3P2",
        {common::Person("http://example.org/F1G2P1")},
        {common::Person("http://example.org/F1G2P2")}
    },
    {
        "TwoFamiliesTheeGenerationsF1G3P3",
        "data/deps_queries/retrieve_person_parents/normal_success_cases/"
        "model-04_two-families-three-generations.ttl",
        "http://example.org/F1G3P3",
        {common::Person("http://example.org/F1G2P1")},
        {common::Person("http://example.org/F1G2P2")}
    },
    {
        "TwoFamiliesTheeGenerationsF1G2P1",
        "data/deps_queries/retrieve_person_parents/normal_success_cases/"
        "model-04_two-families-three-generations.ttl",
        "http://example.org/F1G2P1",
        {common::Person("http://example.org/F1G1P1")},
        {common::Person("http://example.org/F1G1P2")}
    },
    {
        "TwoFamiliesTheeGenerationsF1G2P2",
        "data/deps_queries/retrieve_person_parents/normal_success_cases/"
        "model-04_two-families-three-generations.ttl",
        "http://example.org/F1G2P2",
        {common::Person("http://example.org/F1G1P3")},
        {common::Person("http://example.org/F1G1P4")}
    },
    {
        "TwoFamiliesTheeGenerationsF1G1P1",
        "data/deps_queries/retrieve_person_parents/normal_success_cases/"
        "model-04_two-families-three-generations.ttl",
        "http://example.org/F1G1P1",
        {},
        {}
    },
    {
        "TwoFamiliesTheeGenerationsF1G1P2",
        "data/deps_queries/retrieve_person_parents/normal_success_cases/"
        "model-04_two-families-three-generations.ttl",
        "http://example.org/F1G1P2",
        {},
        {}
    },
    {
        "TwoFamiliesTheeGenerationsF1G1P3",
        "data/deps_queries/retrieve_person_parents/normal_success_cases/"
        "model-04_two-families-three-generations.ttl",
        "http://example.org/F1G1P3",
        {},
        {}
    },
    {
        "TwoFamiliesTheeGenerationsF1G1P4",
        "data/deps_queries/retrieve_person_parents/normal_success_cases/"
        "model-04_two-families-three-generations.ttl",
        "http://example.org/F1G1P4",
        {},
        {}
    },
    {
        "TwoFamiliesTheeGenerationsF2G3P1",
        "data/deps_queries/retrieve_person_parents/normal_success_cases/"
        "model-04_two-families-three-generations.ttl",
        "http://example.org/F2G3P1",
        {common::Person("http://example.org/F2G2P1")},
        {common::Person("http://example.org/F2G2P2")}
    },
    {
        "TwoFamiliesTheeGenerationsF2G3P2",
        "data/deps_queries/retrieve_person_parents/normal_success_cases/"
        "model-04_two-families-three-generations.ttl",
        "http://example.org/F2G3P2",
        {common::Person("http://example.org/F2G2P1")},
        {common::Person("http://example.org/F2G2P2")}
    },
    {
        "TwoFamiliesTheeGenerationsF2G3P3",
        "data/deps_queries/retrieve_person_parents/normal_success_cases/"
        "model-04_two-families-three-generations.ttl",
        "http://example.org/F2G3P3",
        {common::Person("http://example.org/F2G2P3")},
        {common::Person("http://example.org/F2G2P4")}
    },
    {
        "TwoFamiliesTheeGenerationsF2G2P1",
        "data/deps_queries/retrieve_person_parents/normal_success_cases/"
        "model-04_two-families-three-generations.ttl",
        "http://example.org/F2G2P1",
        {common::Person("http://example.org/F2G1P1")},
        {common::Person("http://example.org/F2G1P2")}
    },
    {
        "TwoFamiliesTheeGenerationsF2G2P2",
        "data/deps_queries/retrieve_person_parents/normal_success_cases/"
        "model-04_two-families-three-generations.ttl",
        "http://example.org/F2G2P2",
        {common::Person("http://example.org/F2G1P3")},
        {common::Person("http://example.org/F2G1P4")}
    },
    {
        "TwoFamiliesTheeGenerationsF2G2P3",
        "data/deps_queries/retrieve_person_parents/normal_success_cases/"
        "model-04_two-families-three-generations.ttl",
        "http://example.org/F2G2P3",
        {common::Person("http://example.org/F2G1P3")},
        {common::Person("http://example.org/F2G1P4")}
    },
    {
        "TwoFamiliesTheeGenerationsF2G2P4",
        "data/deps_queries/retrieve_person_parents/normal_success_cases/"
        "model-04_two-families-three-generations.ttl",
        "http://example.org/F2G2P4",
        {common::Person("http://example.org/F2G1P5")},
        {common::Person("http://example.org/F2G1P6")}
    },
    {
        "TwoFamiliesTheeGenerationsF2G1P1",
        "data/deps_queries/retrieve_person_parents/normal_success_cases/"
        "model-04_two-families-three-generations.ttl",
        "http://example.org/F2G1P1",
        {},
        {}
    },
    {
        "TwoFamiliesTheeGenerationsF2G1P2",
        "data/deps_queries/retrieve_person_parents/normal_success_cases/"
        "model-04_two-families-three-generations.ttl",
        "http://example.org/F2G1P2",
        {},
        {}
    },
    {
        "TwoFamiliesTheeGenerationsF2G1P3",
        "data/deps_queries/retrieve_person_parents/normal_success_cases/"
        "model-04_two-families-three-generations.ttl",
        "http://example.org/F2G1P3",
        {},
        {}
    },
    {
        "TwoFamiliesTheeGenerationsF2G1P4",
        "data/deps_queries/retrieve_person_parents/normal_success_cases/"
        "model-04_two-families-three-generations.ttl",
        "http://example.org/F2G1P4",
        {},
        {}
    },
    {
        "TwoFamiliesTheeGenerationsF2G1P5",
        "data/deps_queries/retrieve_person_parents/normal_success_cases/"
        "model-04_two-families-three-generations.ttl",
        "http://example.org/F2G1P5",
        {},
        {}
    },
    {
        "TwoFamiliesTheeGenerationsF2G1P6",
        "data/deps_queries/retrieve_person_parents/normal_success_cases/"
        "model-04_two-families-three-generations.ttl",
        "http://example.org/F2G1P6",
        {},
        {}
    }
};

std::string ParamNameGen(const ::testing::TestParamInfo<Param>& info)
{
    return { info.param.case_name };
}


INSTANTIATE_TEST_SUITE_P(
    ,
    DetailsQueries_RetrievePersonParents,
    ::testing::ValuesIn(g_params),
    ParamNameGen);

// Check if the retrieve_person_father_opt and the retrieve_person_mother_opt functions throw the
//  person_exception when any of their arguments are null.
TEST_F(DetailsQueries_RetrievePersonParents, InputContractViolations)
{
    tools::scoped_redland_ctx ctx = tools::initialize_redland_ctx();
    const auto person = std::make_shared<common::Person>("http://example.org/someone");

    EXPECT_THROW_WITH_CODE(
        person::retrieve_person_father_opt(nullptr, ctx->world, ctx->model),
        person::person_exception, person::person_exception::error_code::input_contract_error);

    EXPECT_THROW_WITH_CODE(
        person::retrieve_person_father_opt(person.get(), nullptr, ctx->model),
        person::person_exception, person::person_exception::error_code::input_contract_error);

    EXPECT_THROW_WITH_CODE(
        person::retrieve_person_father_opt(person.get(), ctx->world, nullptr),
        person::person_exception, person::person_exception::error_code::input_contract_error);

    EXPECT_THROW_WITH_CODE(
        person::retrieve_person_mother_opt(nullptr, ctx->world, ctx->model),
        person::person_exception, person::person_exception::error_code::input_contract_error);

    EXPECT_THROW_WITH_CODE(
        person::retrieve_person_mother_opt(person.get(), nullptr, ctx->model),
        person::person_exception, person::person_exception::error_code::input_contract_error);

    EXPECT_THROW_WITH_CODE(
        person::retrieve_person_mother_opt(person.get(), ctx->world, nullptr),
        person::person_exception, person::person_exception::error_code::input_contract_error);
}

TEST_F(DetailsQueries_RetrievePersonParents, MultipleResourcesFoundError)
{
    tools::scoped_redland_ctx ctx = tools::initialize_redland_ctx();
    const char* data_path = "data/deps_queries/retrieve_person_parents/normal_success_cases/"
        "model-05_multiple-resources-found-error.ttl";
    tools::load_rdf(ctx->world, ctx->model, tools::get_program_path() / data_path);

    const auto proband = std::make_shared<common::Person>("http://example.org/P1");

    EXPECT_THROW_WITH_CODE(
        person::retrieve_person_father_opt(proband.get(), ctx->world, ctx->model),
        person::person_exception, person::person_exception::error_code::multiple_resources_found);

    EXPECT_THROW_WITH_CODE(
        person::retrieve_person_mother_opt(proband.get(), ctx->world, ctx->model),
        person::person_exception, person::person_exception::error_code::multiple_resources_found);
}

} // namespace suite2

} // namespace test
