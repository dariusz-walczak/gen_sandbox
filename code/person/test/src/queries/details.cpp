#include <algorithm>
#include <memory>
#include <variant>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "test/tools/application.hpp"
#include "test/tools/assertions.hpp"
#include "test/tools/error.hpp"
#include "test/tools/gtest.hpp"
#include "test/tools/matchers.hpp"
#include "test/tools/note.hpp"
#include "test/tools/person.hpp"
#include "test/tools/printers.hpp"
#include "test/tools/redland.hpp"
#include "test/tools/resource.hpp"

#include "test/tools/person/comparable_note_factory.hpp"

#include "common/comparators.hpp"
#include "person/error.hpp"
#include "person/queries/details.hpp"

namespace test
{

namespace
{

const bool g_inferred = true;
const bool g_stated = false;

struct ComparablePartnerRelation
{
    common::Person partner;
    bool is_inferred;

    bool operator==(const ComparablePartnerRelation& other) const {
        return ((partner == other.partner) && (is_inferred == other.is_inferred));
    }

    friend void PrintTo(const ComparablePartnerRelation& rel, std::ostream* os) {
        *os << "ComparablePartnerRelation{partner: ";
        tools::PrintTo(tools::to_comparable(rel.partner), os);
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

namespace test::suite_retrieve_person_partners
{

using namespace tools::person;

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

    const auto actual_partners = adapt(
        person::retrieve_person_partners(
            proband.get(), ctx->world, ctx->model, actual_notes));

    EXPECT_EQ(param.expected_partners, actual_partners);
    EXPECT_EQ(param.expected_notes, tools::to_comparable(actual_notes));
};

const std::vector<Param> g_other_cases
{
    {
        .case_name="NoPartner",
        .data_file="data/queries/details/retrieve_person_partners/model-00_no-partner.ttl",
        .proband_uri="http://example.org/P1",
        .expected_partners={
        },
        .expected_notes={
        }
    }
};

/**
 * @brief Suite of single couple test cases
 *
 * For details see the comment in the data file */
const std::vector<Param> g_one_couple_cases
{
    {
        .case_name="OneCoupleVariant01",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V01P1",
        .expected_partners={},
        .expected_notes={
            create_invalid_stated_partner_comparable_note("http://example.org/V01P2")
        }
    },
    {
        .case_name="OneCoupleVariant02",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V02P1",
        .expected_partners={},
        .expected_notes={
            create_invalid_stated_partner_comparable_note("http://example.org/V02P2")
        }
    },
    {
        .case_name="OneCoupleVariant03",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V03P1",
        .expected_partners={},
        .expected_notes={
            create_invalid_stated_partner_comparable_note("http://example.org/V03P2")
        }
    },
    {
        .case_name="OneCoupleVariant04_V04P1",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V04P1",
        .expected_partners={
            {common::Person("http://example.org/V04P2"), g_stated}
        },
        .expected_notes={}
    },
    {
        .case_name="OneCoupleVariant04_V04P2",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V04P2",
        .expected_partners={
            {common::Person("http://example.org/V04P1"), g_stated}
        },
        .expected_notes={}
    },
    {
        .case_name="OneCoupleVariant05",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V05P1",
        .expected_partners={},
        .expected_notes={
            create_invalid_inferred_partner_comparable_note("http://example.org/V05P2")
        }
    },
    {
        .case_name="OneCoupleVariant06",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V06P1",
        .expected_partners={},
        .expected_notes={
            create_invalid_inferred_partner_comparable_note("http://example.org/V06P2")
        }
    },
    {
        .case_name="OneCoupleVariant07",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V07P1",
        .expected_partners={},
        .expected_notes={
            create_invalid_inferred_partner_comparable_note("http://example.org/V07P2")
        }
    },
    {
        .case_name="OneCoupleVariant08",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V08P1",
        .expected_partners={},
        .expected_notes={
            create_invalid_inferred_partner_comparable_note("http://example.org/V08P2")
        }
    },
    {
        .case_name="OneCoupleVariant09",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V09P1",
        .expected_partners={},
        .expected_notes={
            create_invalid_inferred_partner_comparable_note("http://example.org/V09P2")
        }
    },
    {
        .case_name="OneCoupleVariant10",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V10P1",
        .expected_partners={},
        .expected_notes={
            create_invalid_inferred_partner_comparable_note("http://example.org/V10P2")
        }
    },
    {
        .case_name="OneCoupleVariant11",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V11P1",
        .expected_partners={},
        .expected_notes={
            create_invalid_inferred_partner_comparable_note("http://example.org/V11P2")
        }
    },
    {
        .case_name="OneCoupleVariant12",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V12P1",
        .expected_partners={},
        .expected_notes={
            create_invalid_inferred_partner_comparable_note("http://example.org/V12P2")
        }
    },
    {
        .case_name="OneCoupleVariant13",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V13P1",
        .expected_partners={},
        .expected_notes={
            create_invalid_inferred_partner_comparable_note("http://example.org/V13P2")
        }
    },
    {
        .case_name="OneCoupleVariant14",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V14P1",
        .expected_partners={},
        .expected_notes={
            create_invalid_inferred_partner_comparable_note("http://example.org/V14P2")
        }
    },
    {
        .case_name="OneCoupleVariant15",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V15P1",
        .expected_partners={},
        .expected_notes={
            create_invalid_inferred_partner_comparable_note("http://example.org/V15P2")
        }
    },
    {
        .case_name="OneCoupleVariant16",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V16P1",
        .expected_partners={},
        .expected_notes={
            create_invalid_inferred_partner_comparable_note("http://example.org/V16P2")
        }
    },
    {
        .case_name="OneCoupleVariant17_V17P1",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V17P1",
        .expected_partners={
            {common::Person("http://example.org/V17P2"), g_inferred}
        },
        .expected_notes={
            create_inferred_partner_comparable_note("http://example.org/V17P2")
        }
    },
    {
        .case_name="OneCoupleVariant17_V17P2",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V17P2",
        .expected_partners={
            {common::Person("http://example.org/V17P1"), g_inferred}
        },
        .expected_notes={
            create_inferred_partner_comparable_note("http://example.org/V17P1")
        }
    },
    {
        .case_name="OneCoupleVariant18_V18P1",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V18P1",
        .expected_partners={
            {common::Person("http://example.org/V18P2"), g_inferred}
        },
        .expected_notes={
            create_inferred_partner_comparable_note("http://example.org/V18P2")
        }
    },
    {
        .case_name="OneCoupleVariant18_V18P2",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V18P2",
        .expected_partners={
            {common::Person("http://example.org/V18P1"), g_inferred}
        },
        .expected_notes={
            create_inferred_partner_comparable_note("http://example.org/V18P1")
        }
    },
    {
        .case_name="OneCoupleVariant19_V19P1",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V19P1",
        .expected_partners={
            {common::Person("http://example.org/V19P2"), g_inferred}
        },
        .expected_notes={
            create_inferred_partner_comparable_note("http://example.org/V19P2")
        }
    },
    {
        .case_name="OneCoupleVariant19_V19P2",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V19P2",
        .expected_partners={
            {common::Person("http://example.org/V19P1"), g_inferred}
        },
        .expected_notes={
            create_inferred_partner_comparable_note("http://example.org/V19P1")
        }
    },
    {
        .case_name="OneCoupleVariant20_V20P1",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V20P1",
        .expected_partners={
            {common::Person("http://example.org/V20P2"), g_inferred}
        },
        .expected_notes={
            create_inferred_partner_comparable_note("http://example.org/V20P2")
        }
    },
    {
        .case_name="OneCoupleVariant20_V20P2",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V20P2",
        .expected_partners={
            {common::Person("http://example.org/V20P1"), g_inferred}
        },
        .expected_notes={
            create_inferred_partner_comparable_note("http://example.org/V20P1")
        }
    },
    {
        .case_name="OneCoupleVariant21",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V21P1",
        .expected_partners={},
        .expected_notes={
            create_invalid_stated_partner_comparable_note("http://example.org/V21P2")
        }
    },
    {
        .case_name="OneCoupleVariant22",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V22P1",
        .expected_partners={},
        .expected_notes={
            create_invalid_stated_partner_comparable_note("http://example.org/V22P2")
        }
    },
    {
        .case_name="OneCoupleVariant23",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V23P1",
        .expected_partners={},
        .expected_notes={
            create_invalid_stated_partner_comparable_note("http://example.org/V23P2")
        }
    },
    {
        .case_name="OneCoupleVariant24",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V24P1",
        .expected_partners={},
        .expected_notes={
            create_invalid_stated_partner_comparable_note("http://example.org/V24P2")
        }
    },
    {
        .case_name="OneCoupleVariant25",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V25P1",
        .expected_partners={},
        .expected_notes={
            create_invalid_stated_partner_comparable_note("http://example.org/V25P2")
        }
    },
    {
        .case_name="OneCoupleVariant26",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V26P1",
        .expected_partners={},
        .expected_notes={
            create_invalid_stated_partner_comparable_note("http://example.org/V26P2")
        }
    },
    {
        .case_name="OneCoupleVariant27",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V27P1",
        .expected_partners={},
        .expected_notes={
            create_invalid_stated_partner_comparable_note("http://example.org/V27P2")
        }
    },
    {
        .case_name="OneCoupleVariant28",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V28P1",
        .expected_partners={},
        .expected_notes={
            create_invalid_stated_partner_comparable_note("http://example.org/V28P2")
        }
    },
    {
        .case_name="OneCoupleVariant29",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V29P1",
        .expected_partners={},
        .expected_notes={
            create_invalid_stated_partner_comparable_note("http://example.org/V29P2")
        }
    },
    {
        .case_name="OneCoupleVariant30",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V30P1",
        .expected_partners={},
        .expected_notes={
            create_invalid_stated_partner_comparable_note("http://example.org/V30P2")
        }
    },
    {
        .case_name="OneCoupleVariant31",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V31P1",
        .expected_partners={},
        .expected_notes={
            create_invalid_stated_partner_comparable_note("http://example.org/V31P2")
        }
    },
    {
        .case_name="OneCoupleVariant32",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V32P1",
        .expected_partners={},
        .expected_notes={
            create_invalid_stated_partner_comparable_note("http://example.org/V32P2")
        }
    },
    {
        .case_name="OneCoupleVariant33_V33P1",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V33P1",
        .expected_partners={
            {common::Person("http://example.org/V33P2"), g_stated}
        },
        .expected_notes={}
    },
    {
        .case_name="OneCoupleVariant33_V33P2",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V33P2",
        .expected_partners={
            {common::Person("http://example.org/V33P1"), g_stated}
        },
        .expected_notes={}
    },
    {
        .case_name="OneCoupleVariant34_V34P1",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V34P1",
        .expected_partners={
            {common::Person("http://example.org/V34P2"), g_stated}
        },
        .expected_notes={}
    },
    {
        .case_name="OneCoupleVariant34_V34P2",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V34P2",
        .expected_partners={
            {common::Person("http://example.org/V34P1"), g_stated}
        },
        .expected_notes={}
    },
    {
        .case_name="OneCoupleVariant35_V35P1",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V35P1",
        .expected_partners={
            {common::Person("http://example.org/V35P2"), g_stated}
        },
        .expected_notes={}
    },
    {
        .case_name="OneCoupleVariant35_V35P2",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V35P2",
        .expected_partners={
            {common::Person("http://example.org/V35P1"), g_stated}
        },
        .expected_notes={}
    },
    {
        .case_name="OneCoupleVariant36_V36P1",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V36P1",
        .expected_partners={
            {common::Person("http://example.org/V36P2"), g_stated}
        },
        .expected_notes={}
    },
    {
        .case_name="OneCoupleVariant36_V36P2",
        .data_file="data/queries/details/retrieve_person_partners/model-01_one-couple.ttl",
        .proband_uri="http://example.org/V36P2",
        .expected_partners={
            {common::Person("http://example.org/V36P1"), g_stated}
        },
        .expected_notes={}
    }
};

const std::vector<Param> g_two_couples_cases
{
    {
        .case_name="TwoCouplesV201",
        .data_file="data/queries/details/retrieve_person_partners/model-02_two-couples.ttl",
        .proband_uri="http://example.org/V201P1",
        .expected_partners={
        },
        .expected_notes={
            create_invalid_stated_partner_comparable_note("http://example.org/V201P2"),
            create_invalid_stated_partner_comparable_note("http://example.org/V201P3")
        }
    },
    {
        .case_name="TwoCouplesV202",
        .data_file="data/queries/details/retrieve_person_partners/model-02_two-couples.ttl",
        .proband_uri="http://example.org/V202P1",
        .expected_partners={
            {common::Person("http://example.org/V202P3"), g_stated}
        },
        .expected_notes={
            create_invalid_stated_partner_comparable_note("http://example.org/V202P2")
        }
    },
    {
        .case_name="TwoCouplesV203",
        .data_file="data/queries/details/retrieve_person_partners/model-02_two-couples.ttl",
        .proband_uri="http://example.org/V203P1",
        .expected_partners={
        },
        .expected_notes={
            create_invalid_stated_partner_comparable_note("http://example.org/V203P2"),
            create_invalid_inferred_partner_comparable_note("http://example.org/V203P3")
        }
    },
    {
        .case_name="TwoCouplesV204",
        .data_file="data/queries/details/retrieve_person_partners/model-02_two-couples.ttl",
        .proband_uri="http://example.org/V204P1",
        .expected_partners={
        },
        .expected_notes={
            create_invalid_stated_partner_comparable_note("http://example.org/V204P2"),
            create_invalid_stated_partner_comparable_note("http://example.org/V204P3")
        }
    },
    {
        .case_name="TwoCouplesV205",
        .data_file="data/queries/details/retrieve_person_partners/model-02_two-couples.ttl",
        .proband_uri="http://example.org/V205P1",
        .expected_partners={
            {common::Person("http://example.org/V205P3"), g_inferred}
        },
        .expected_notes={
            create_invalid_stated_partner_comparable_note("http://example.org/V205P2"),
            create_inferred_partner_comparable_note("http://example.org/V205P3")
        }
    },
    {
        .case_name="TwoCouplesV206",
        .data_file="data/queries/details/retrieve_person_partners/model-02_two-couples.ttl",
        .proband_uri="http://example.org/V206P1",
        .expected_partners={
            {common::Person("http://example.org/V206P3"), g_stated}
        },
        .expected_notes={
            create_invalid_stated_partner_comparable_note("http://example.org/V206P2")
        }
    },
    {
        .case_name="TwoCouplesV207",
        .data_file="data/queries/details/retrieve_person_partners/model-02_two-couples.ttl",
        .proband_uri="http://example.org/V207P1",
        .expected_partners={
            {common::Person("http://example.org/V207P2"), g_stated},
            {common::Person("http://example.org/V207P3"), g_stated}
        },
        .expected_notes={
        }
    },
    {
        .case_name="TwoCouplesV208",
        .data_file="data/queries/details/retrieve_person_partners/model-02_two-couples.ttl",
        .proband_uri="http://example.org/V208P1",
        .expected_partners={
            {common::Person("http://example.org/V208P2"), g_stated}
        },
        .expected_notes={
            create_invalid_inferred_partner_comparable_note("http://example.org/V208P3")
        }
    },
    {
        .case_name="TwoCouplesV209",
        .data_file="data/queries/details/retrieve_person_partners/model-02_two-couples.ttl",
        .proband_uri="http://example.org/V209P1",
        .expected_partners={
            {common::Person("http://example.org/V209P2"), g_stated}
        },
        .expected_notes={
            create_invalid_stated_partner_comparable_note("http://example.org/V209P3")
        }
    },
    {
        .case_name="TwoCouplesV210",
        .data_file="data/queries/details/retrieve_person_partners/model-02_two-couples.ttl",
        .proband_uri="http://example.org/V210P1",
        .expected_partners={
            {common::Person("http://example.org/V210P2"), g_stated},
            {common::Person("http://example.org/V210P3"), g_inferred}
        },
        .expected_notes={
            create_inferred_partner_comparable_note("http://example.org/V210P3")
        }
    },
    {
        .case_name="TwoCouplesV211",
        .data_file="data/queries/details/retrieve_person_partners/model-02_two-couples.ttl",
        .proband_uri="http://example.org/V211P1",
        .expected_partners={
            {common::Person("http://example.org/V211P2"), g_stated},
            {common::Person("http://example.org/V211P3"), g_stated}
        },
        .expected_notes={
        }
    },
    {
        .case_name="TwoCouplesV212",
        .data_file="data/queries/details/retrieve_person_partners/model-02_two-couples.ttl",
        .proband_uri="http://example.org/V212P1",
        .expected_partners={
        },
        .expected_notes={
            create_invalid_inferred_partner_comparable_note("http://example.org/V212P2"),
            create_invalid_inferred_partner_comparable_note("http://example.org/V212P3")
        }
    },
    {
        .case_name="TwoCouplesV213",
        .data_file="data/queries/details/retrieve_person_partners/model-02_two-couples.ttl",
        .proband_uri="http://example.org/V213P1",
        .expected_partners={
        },
        .expected_notes={
            create_invalid_inferred_partner_comparable_note("http://example.org/V213P2"),
            create_invalid_stated_partner_comparable_note("http://example.org/V213P3")
        }
    },
    {
        .case_name="TwoCouplesV214",
        .data_file="data/queries/details/retrieve_person_partners/model-02_two-couples.ttl",
        .proband_uri="http://example.org/V214P1",
        .expected_partners={
            {common::Person("http://example.org/V214P3"), g_inferred}
        },
        .expected_notes={
            create_invalid_inferred_partner_comparable_note("http://example.org/V214P2"),
            create_inferred_partner_comparable_note("http://example.org/V214P3")
        }
    },
    {
        .case_name="TwoCouplesV215",
        .data_file="data/queries/details/retrieve_person_partners/model-02_two-couples.ttl",
        .proband_uri="http://example.org/V215P1",
        .expected_partners={
            {common::Person("http://example.org/V215P3"), g_stated}
        },
        .expected_notes={
            create_invalid_inferred_partner_comparable_note("http://example.org/V215P2"),
        }
    },
    {
        .case_name="TwoCouplesV216",
        .data_file="data/queries/details/retrieve_person_partners/model-02_two-couples.ttl",
        .proband_uri="http://example.org/V216P1",
        .expected_partners={
        },
        .expected_notes={
            create_invalid_stated_partner_comparable_note("http://example.org/V216P2"),
            create_invalid_stated_partner_comparable_note("http://example.org/V216P3"),
        }
    },
    {
        .case_name="TwoCouplesV217",
        .data_file="data/queries/details/retrieve_person_partners/model-02_two-couples.ttl",
        .proband_uri="http://example.org/V217P1",
        .expected_partners={
            {common::Person("http://example.org/V217P3"), g_inferred}
        },
        .expected_notes={
            create_invalid_stated_partner_comparable_note("http://example.org/V217P2"),
            create_inferred_partner_comparable_note("http://example.org/V217P3"),
        }
    },
    {
        .case_name="TwoCouplesV218",
        .data_file="data/queries/details/retrieve_person_partners/model-02_two-couples.ttl",
        .proband_uri="http://example.org/V218P1",
        .expected_partners={
            {common::Person("http://example.org/V218P3"), g_stated}
        },
        .expected_notes={
            create_invalid_stated_partner_comparable_note("http://example.org/V218P2"),
        }
    },
    {
        .case_name="TwoCouplesV219",
        .data_file="data/queries/details/retrieve_person_partners/model-02_two-couples.ttl",
        .proband_uri="http://example.org/V219P1",
        .expected_partners={
            {common::Person("http://example.org/V219P2"), g_inferred},
            {common::Person("http://example.org/V219P3"), g_inferred}
        },
        .expected_notes={
            create_inferred_partner_comparable_note("http://example.org/V219P2"),
            create_inferred_partner_comparable_note("http://example.org/V219P3")
        }
    },
    {
        .case_name="TwoCouplesV220",
        .data_file="data/queries/details/retrieve_person_partners/model-02_two-couples.ttl",
        .proband_uri="http://example.org/V220P1",
        .expected_partners={
            {common::Person("http://example.org/V220P2"), g_inferred},
            {common::Person("http://example.org/V220P3"), g_stated}
        },
        .expected_notes={
            create_inferred_partner_comparable_note("http://example.org/V220P2")
        }
    },
    {
        .case_name="TwoCouplesV221",
        .data_file="data/queries/details/retrieve_person_partners/model-02_two-couples.ttl",
        .proband_uri="http://example.org/V221P1",
        .expected_partners={
            {common::Person("http://example.org/V221P2"), g_stated},
            {common::Person("http://example.org/V221P3"), g_stated}
        },
        .expected_notes={
        }
    },
    ///////////////////////////////////////////////////////////////////////////////////////////////
    {
        .case_name="ThreeGensG1P1",
        .data_file="data/queries/details/retrieve_person_partners/model-03_three-generations.ttl",
        .proband_uri="http://example.org/G1P1",
        .expected_partners={
            {common::Person("http://example.org/G1P2"), g_stated}
        },
        .expected_notes={
        }
    },
    {
        .case_name="ThreeGensG2P1",
        .data_file="data/queries/details/retrieve_person_partners/model-03_three-generations.ttl",
        .proband_uri="http://example.org/G2P1",
        .expected_partners={
            {common::Person("http://example.org/G2P2"), g_stated}
        },
        .expected_notes={
        }
    },
    {
        .case_name="ThreeGensG3P1",
        .data_file="data/queries/details/retrieve_person_partners/model-03_three-generations.ttl",
        .proband_uri="http://example.org/G3P1",
        .expected_partners={
        },
        .expected_notes={
        }
    },
    {
        .case_name="OWL84_P00010",
        .data_file="data/queries/details/retrieve_person_partners/model-04_owl-84-repro.ttl",
        .proband_uri="http://example.org/P00010",
        .expected_partners={
            {common::Person("http://example.org/P00000"), g_stated}
        },
        .expected_notes={
        }
    },
    {
        .case_name="ThreeGensInferredG1P1",
        .data_file=("data/queries/details/retrieve_person_partners/"
                    "model-05_three-generations-some-inferred.ttl"),
        .proband_uri="http://example.org/G1P1",
        .expected_partners={
            {common::Person("http://example.org/G1P2"), g_inferred}
        },
        .expected_notes={
            create_inferred_partner_comparable_note("http://example.org/G1P2")
        }
    },
    {
        .case_name="ThreeGensInferredG1P2",
        .data_file=("data/queries/details/retrieve_person_partners/"
                    "model-05_three-generations-some-inferred.ttl"),
        .proband_uri="http://example.org/G1P2",
        .expected_partners={
            {common::Person("http://example.org/G1P1"), g_inferred}
        },
        .expected_notes={
            create_inferred_partner_comparable_note("http://example.org/G1P1")
        }
    },
    {
        .case_name="ThreeGensInferredG1P3",
        .data_file=("data/queries/details/retrieve_person_partners/"
                    "model-05_three-generations-some-inferred.ttl"),
        .proband_uri="http://example.org/G1P3",
        .expected_partners={
            {common::Person("http://example.org/G1P4"), g_stated}
        },
        .expected_notes={
        }
    },
    {
        .case_name="ThreeGensInferredG2P1",
        .data_file=("data/queries/details/retrieve_person_partners/"
                    "model-05_three-generations-some-inferred.ttl"),
        .proband_uri="http://example.org/G2P1",
        .expected_partners={
            {common::Person("http://example.org/G2P2"), g_inferred}
        },
        .expected_notes={
            create_inferred_partner_comparable_note("http://example.org/G2P2")
        }
    },
    {
        .case_name="ThreeGensInferredG2P2",
        .data_file=("data/queries/details/retrieve_person_partners/"
                    "model-05_three-generations-some-inferred.ttl"),
        .proband_uri="http://example.org/G2P2",
        .expected_partners={
            {common::Person("http://example.org/G2P1"), g_inferred}
        },
        .expected_notes={
            create_inferred_partner_comparable_note("http://example.org/G2P1")
        }
    },
    {
        .case_name="ThreeGensInferredG3P1",
        .data_file=("data/queries/details/retrieve_person_partners/"
                    "model-05_three-generations-some-inferred.ttl"),
        .proband_uri="http://example.org/G3P1",
        .expected_partners={},
        .expected_notes={}
    },
    {
        .case_name="PartnerChain_P1",
        .data_file="data/queries/details/retrieve_person_partners/model-06_partner-chain.ttl",
        .proband_uri="http://example.org/P1",
        .expected_partners={
            {common::Person("http://example.org/P2"), g_stated}
        },
        .expected_notes={
        }
    },
    {
        .case_name="PartnerChain_P2",
        .data_file="data/queries/details/retrieve_person_partners/model-06_partner-chain.ttl",
        .proband_uri="http://example.org/P2",
        .expected_partners={
            {common::Person("http://example.org/P1"), g_stated},
            {common::Person("http://example.org/P3"), g_stated},
        },
        .expected_notes={
        }
    },
    {
        .case_name="PartnerChain_P3",
        .data_file="data/queries/details/retrieve_person_partners/model-06_partner-chain.ttl",
        .proband_uri="http://example.org/P3",
        .expected_partners={
            {common::Person("http://example.org/P2"), g_stated},
            {common::Person("http://example.org/P4"), g_stated}
        },
        .expected_notes={
        }
    },
    {
        .case_name="PartnerChain_P4",
        .data_file="data/queries/details/retrieve_person_partners/model-06_partner-chain.ttl",
        .proband_uri="http://example.org/P4",
        .expected_partners={
            {common::Person("http://example.org/P3"), g_stated}
        },
        .expected_notes={
        }
    },
    //////////////////////////////////////////////////////////////////////////////////////////////
    {
        .case_name="TwoInvalidStatedPartners",
        .data_file="data/queries/details/retrieve_person_partners/model-06_invalid-partners.ttl",
        .proband_uri="http://example.org/F2P2",
        .expected_partners={},
        .expected_notes={
            create_invalid_stated_partner_comparable_note("http://example.org/F2P1"),
            create_invalid_stated_partner_comparable_note("http://example.org/F2P3"),
        }
    },
    {
        .case_name="OneInvalidOneValidStatedPartner",
        .data_file="data/queries/details/retrieve_person_partners/model-06_invalid-partners.ttl",
        .proband_uri="http://example.org/F3P2",
        .expected_partners={
            {common::Person("http://example.org/F3P1"), g_stated}
        },
        .expected_notes={
            create_invalid_stated_partner_comparable_note("http://example.org/F3P3"),
        }
    },
    {
        .case_name="OneInvalidInferredPartner",
        .data_file="data/queries/details/retrieve_person_partners/model-06_invalid-partners.ttl",
        .proband_uri="http://example.org/F4P2",
        .expected_partners={},
        .expected_notes={
            create_invalid_inferred_partner_comparable_note("http://example.org/F4P1")
        }
    },
    {
        .case_name="TwoInvalidInferredPartners",
        .data_file="data/queries/details/retrieve_person_partners/model-06_invalid-partners.ttl",
        .proband_uri="http://example.org/F5P2",
        .expected_partners={},
        .expected_notes={
            create_invalid_inferred_partner_comparable_note("http://example.org/F5P1"),
            create_invalid_inferred_partner_comparable_note("http://example.org/F5P3")
        }
    },
    {
        .case_name="OneInvalidTwiceInferredPartnerF6P1",
        .data_file="data/queries/details/retrieve_person_partners/model-06_invalid-partners.ttl",
        .proband_uri="http://example.org/F6P1",
        .expected_partners={},
        .expected_notes={
            create_invalid_inferred_partner_comparable_note("http://example.org/F6P2")
        }
    },
    {
        .case_name="OneInvalidTwiceInferredPartnerF6P3",
        .data_file="data/queries/details/retrieve_person_partners/model-06_invalid-partners.ttl",
        .proband_uri="http://example.org/F6P3",
        .expected_partners={},
        .expected_notes={
            create_invalid_inferred_partner_comparable_note("http://example.org/F6P2")
        }
    },
    {
        .case_name="OneInvalidOneValidInferredPartner",
        .data_file="data/queries/details/retrieve_person_partners/model-06_invalid-partners.ttl",
        .proband_uri="http://example.org/F7P2",
        .expected_partners={
            {common::Person("http://example.org/F7P3"), g_inferred}
        },
        .expected_notes={
            create_invalid_inferred_partner_comparable_note("http://example.org/F7P1"),
            create_inferred_partner_comparable_note("http://example.org/F7P3")
        }
    }
};

std::string ParamNameGen(const ::testing::TestParamInfo<Param>& info)
{
    return { info.param.case_name };
}


INSTANTIATE_TEST_SUITE_P(
    OtherCases,
    DetailsQueries_RetrievePersonPartners,
    ::testing::ValuesIn(g_other_cases),
    ParamNameGen);

INSTANTIATE_TEST_SUITE_P(
    OneCoupleCases,
    DetailsQueries_RetrievePersonPartners,
    ::testing::ValuesIn(g_one_couple_cases),
    ParamNameGen);

INSTANTIATE_TEST_SUITE_P(
    TwoCouplesCases,
    DetailsQueries_RetrievePersonPartners,
    ::testing::ValuesIn(g_two_couples_cases),
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

} // namespace test::suite_retrieve_person_partners

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

} // namespace suite2

} // namespace test
