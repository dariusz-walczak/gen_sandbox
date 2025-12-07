#include <set>
#include <string>
#include <vector>

#include <gtest/gtest.h>
#include <boost/url.hpp>

#include "person/error.hpp"
#include "person/queries/common.hpp"

#include "test/tools/application.hpp"
#include "test/tools/assertions.hpp"
#include "test/tools/gtest.hpp"
#include "test/tools/person.hpp"
#include "test/tools/redland.hpp"

//  The retrieve_person_uris function tests
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

namespace test::suite_retrieve_person_uris
{

struct Param
{
    const char* case_name;

    struct triple
    {
        const char* subject;
        const char* predicate;
        const char* object;
    };

    std::vector<triple> triples;
    std::set<std::string> iris;
};

class CommonQueries_RetrievePersonIris : public ::testing::TestWithParam<Param> {};

TEST_P(CommonQueries_RetrievePersonIris, NormalSuccessCases)
{
    const Param& param = GetParam();
    test::tools::scoped_redland_ctx ctx = test::tools::initialize_redland_ctx();

    for (const auto& triple : param.triples)
    {
        test::tools::insert_uuu_statement(
            ctx->world, ctx->model,
            triple.subject, triple.predicate, triple.object);
    }

    common::resource_set resources = person::retrieve_person_uris(ctx->world, ctx->model);

    const std::set<boost::urls::url_view> expected_iris{ param.iris.begin(), param.iris.end() };
    std::set<boost::urls::url_view> actual_iris;

    std::transform(
        resources.begin(), resources.end(), std::inserter(actual_iris, actual_iris.end()),
        [](auto const& r) { return r->get_uri(); });

    EXPECT_EQ(expected_iris, actual_iris);
}

const std::vector<Param> g_params {
    {
        "EmptyModel",
        {
        },
        {
        }
    },
    {
        "SinglePersonTriple",
        {
            {
                "http://example.com/P00001",
                "http://www.w3.org/1999/02/22-rdf-syntax-ns#type",
                "http://gedcomx.org/Person"
            }
        },
        {
            "http://example.com/P00001"
        }
    },
    {
        "TwoPersonTriples",
        {
            {
                "http://example.com/P00011",
                "http://www.w3.org/1999/02/22-rdf-syntax-ns#type",
                "http://gedcomx.org/Person"
            },
            {
                "http://example.com/P00012",
                "http://www.w3.org/1999/02/22-rdf-syntax-ns#type",
                "http://gedcomx.org/Person"
            }
        },
        {
            { "http://example.com/P00011" },
            { "http://example.com/P00012" }
        }
    },
    {
        "SingleNonPersonTriple",
        {
            {
                "http://example.com/R00021",
                "http://www.w3.org/1999/02/22-rdf-syntax-ns#type",
                "http://gedcomx.org/Relationship"
            }
        },
        {
        }
    },
    {
        "TwoPersonAndSomeNonPersonTriples",
        {
            {
                "http://example.com/R00031",
                "http://www.w3.org/1999/02/22-rdf-syntax-ns#type",
                "http://gedcomx.org/Relationship"
            },
            {
                "http://example.com/R00031",
                "http://gedcomx.org/type"
                "http://gedcomx.org/Couple"
            },
            {
                "http://example.com/R00031",
                "http://gedcomx.org/person1"
                "http://example.com/P00032"
            },
            {
                "http://example.com/R00031",
                "http://gedcomx.org/person2"
                "http://example.com/P00042"
            },
            {
                "http://example.com/P00032",
                "http://www.w3.org/1999/02/22-rdf-syntax-ns#type",
                "http://gedcomx.org/Person"
            },
            {
                "http://example.com/P00042",
                "http://www.w3.org/1999/02/22-rdf-syntax-ns#type",
                "http://gedcomx.org/Person"
            }
        },
        {
            "http://example.com/P00032",
            "http://example.com/P00042"
        }
    }
};


INSTANTIATE_TEST_SUITE_P(
    ,
    CommonQueries_RetrievePersonIris,
    ::testing::ValuesIn(g_params),
    tools::ParamNameGen<Param>
    );

} // namespace test::suite_retrieve_person_uris


//  The retrieve_person_caption_data_opt, and retrieve_person_caption_data_req tests
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

namespace test::suite_retrieve_person_caption_data
{

struct Param
{
    const char* case_name;
    const char* data_file;
    const char* proband_uri;

    std::optional<tools::ComparablePerson> expected_person;
};

class CommonQueries_RetrievePersonCaptionDataOpt : public ::testing::TestWithParam<Param> {};

TEST_P(CommonQueries_RetrievePersonCaptionDataOpt, NormalSuccessCases)
{
    const Param& param = GetParam();
    tools::scoped_redland_ctx ctx = tools::initialize_redland_ctx();
    tools::load_rdf(ctx->world, ctx->model, tools::get_program_path() / param.data_file);

    const std::shared_ptr<common::Person> actual_person =
        person::retrieve_person_caption_data_opt(param.proband_uri, ctx->world, ctx->model);

    if (param.expected_person.has_value())
    {
        EXPECT_EQ(param.expected_person, tools::to_comparable(*actual_person));
    }
    else
    {
        EXPECT_EQ(actual_person, nullptr);
    }
};

const std::vector<Param> g_params {
    {
        .case_name="ExistingPersonWithName",
        .data_file=("data/queries/common/retrieve_person_caption_data/"
                    "model-01_normal-success-cases.ttl"),
        .proband_uri="http://example.org/Person2",
        .expected_person={
            {
                .id="http://example.org/Person2",
                .caption="Navickas, Domantas"
            }
        }
    },
    {
        .case_name="ExistingPersonWithoutGiven",
        .data_file=("data/queries/common/retrieve_person_caption_data/"
                    "model-01_normal-success-cases.ttl"),
        .proband_uri="http://example.org/Person4",
        .expected_person={
            {
                .id="http://example.org/Person4",
                .caption="Kazlauskaitė"
            }
        }
    },
    {
        .case_name="ExistingPersonWithoutSurname",
        .data_file=("data/queries/common/retrieve_person_caption_data/"
                    "model-01_normal-success-cases.ttl"),
        .proband_uri="http://example.org/Person3",
        .expected_person={
            {
                .id="http://example.org/Person3",
                .caption="Justinas"
            }
        }
    },
    {
        .case_name="ExistingPersonWithoutName",
        .data_file=("data/queries/common/retrieve_person_caption_data/"
                    "model-01_normal-success-cases.ttl"),
        .proband_uri="http://example.org/Person7",
        .expected_person={
            {
                .id="http://example.org/Person7",
                .caption=""
            }
        }
    },
    {
        .case_name="MissingPerson",
        .data_file=("data/queries/common/retrieve_person_caption_data/"
                    "model-01_normal-success-cases.ttl"),
        .proband_uri="http://example.org/Missing",
        .expected_person={}
    }
};

INSTANTIATE_TEST_SUITE_P(
    ,
    CommonQueries_RetrievePersonCaptionDataOpt,
    ::testing::ValuesIn(g_params),
    tools::ParamNameGen<Param>);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

class CommonQueries_RetrievePersonCaptionDataReq : public ::testing::TestWithParam<Param> {};

TEST_P(CommonQueries_RetrievePersonCaptionDataReq, AllCases)
{
    const Param& param = GetParam();
    tools::scoped_redland_ctx ctx = tools::initialize_redland_ctx();
    tools::load_rdf(ctx->world, ctx->model, tools::get_program_path() / param.data_file);

    if (param.expected_person.has_value())
    {
        const std::shared_ptr<common::Person> actual_person =
            person::retrieve_person_caption_data_req(param.proband_uri, ctx->world, ctx->model);

        EXPECT_EQ(param.expected_person, tools::to_comparable(*actual_person));
    }
    else
    {
        EXPECT_THROW_WITH_CODE(
            person::retrieve_person_caption_data_req(param.proband_uri, ctx->world, ctx->model),
            person::person_exception, person::person_exception::error_code::resource_not_found);
    }
};

INSTANTIATE_TEST_SUITE_P(
    ,
    CommonQueries_RetrievePersonCaptionDataReq,
    ::testing::ValuesIn(g_params),
    tools::ParamNameGen<Param>);

} // namespace test::suite_retrieve_person_caption_data

// The retrieve_person_caption_data_seq_req function tests
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

namespace test::suite_retrieve_person_caption_data_seq
{

struct Param
{
    const char* case_name;
    const char* data_file;
    const std::vector<std::string> input_uri_seq;

    const std::vector<tools::ComparablePerson> expected_person_seq;
};

class CommonQueries_RetrievePersonCaptionDataSeqReq_ValidInput : public ::testing::TestWithParam<Param> {};

TEST_P(CommonQueries_RetrievePersonCaptionDataSeqReq_ValidInput, NormalSuccessCases)
{
    const Param& param = GetParam();
    tools::scoped_redland_ctx ctx = tools::initialize_redland_ctx();
    tools::load_rdf(ctx->world, ctx->model, tools::get_program_path() / param.data_file);

    const std::vector<std::shared_ptr<common::Person>> actual_person_seq =
        person::retrieve_person_caption_data_seq_req(param.input_uri_seq, ctx->world, ctx->model);

    EXPECT_EQ(param.expected_person_seq, tools::to_comparable(actual_person_seq));
};

const std::vector<Param> g_params {
    {
        .case_name="EmptyInput",
        .data_file=("data/queries/common/retrieve_person_caption_data/"
                    "model-01_normal-success-cases.ttl"),
        .input_uri_seq={},
        .expected_person_seq={}
    },
    {
        .case_name="TwoPersons",
        .data_file=("data/queries/common/retrieve_person_caption_data/"
                    "model-01_normal-success-cases.ttl"),
        .input_uri_seq={
            "http://example.org/Person2",
            "http://example.org/Person1",
        },
        .expected_person_seq={
            {
                .id="http://example.org/Person2",
                .caption="Navickas, Domantas"
            },
            {
                .id="http://example.org/Person1",
                .caption="Petrauskaitė, Ugnė"
            }
        }
    },
    {
        .case_name="SevenPersons",
        .data_file=("data/queries/common/retrieve_person_caption_data/"
                    "model-01_normal-success-cases.ttl"),
        .input_uri_seq={
            "http://example.org/Person1",
            "http://example.org/Person2",
            "http://example.org/Person3",
            "http://example.org/Person4",
            "http://example.org/Person5",
            "http://example.org/Person6",
            "http://example.org/Person7"
        },
        .expected_person_seq={
            {
                .id="http://example.org/Person1",
                .caption="Petrauskaitė, Ugnė"
            },
            {
                .id="http://example.org/Person2",
                .caption="Navickas, Domantas"
            },
            {
                .id="http://example.org/Person3",
                .caption="Justinas"
            },
            {
                .id="http://example.org/Person4",
                .caption="Kazlauskaitė"
            },
            {
                .id="http://example.org/Person5",
                .caption="Jankauskaitė, Austėja"
            },
            {
                .id="http://example.org/Person6",
                .caption="Jankauskas, Matas"
            },
            {
                .id="http://example.org/Person7",
                .caption=""
            },
        }
    }
};

INSTANTIATE_TEST_SUITE_P(
    ,
    CommonQueries_RetrievePersonCaptionDataSeqReq_ValidInput,
    ::testing::ValuesIn(g_params),
    tools::ParamNameGen<Param>);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

class CommonQueries_RetrievePersonCaptionDataSeqReq_InvalidInput
    : public ::testing::TestWithParam<Param> {};

TEST_P(CommonQueries_RetrievePersonCaptionDataSeqReq_InvalidInput, MissingPersonErrorCases)
{
    const Param& param = GetParam();
    tools::scoped_redland_ctx ctx = tools::initialize_redland_ctx();
    tools::load_rdf(ctx->world, ctx->model, tools::get_program_path() / param.data_file);


        EXPECT_THROW_WITH_CODE(
            person::retrieve_person_caption_data_seq_req(
                param.input_uri_seq, ctx->world, ctx->model),
            person::person_exception, person::person_exception::error_code::resource_not_found);
};

const std::vector<Param> g_missing_person_error_params {
    {
        .case_name="SingleMissing",
        .data_file=("data/queries/common/retrieve_person_caption_data/"
                    "model-01_normal-success-cases.ttl"),
        .input_uri_seq={
            "http://example.org/MissingPerson"
        }
    },
    {
        .case_name="TwoMissingOneExisting",
        .data_file=("data/queries/common/retrieve_person_caption_data/"
                    "model-01_normal-success-cases.ttl"),
        .input_uri_seq={
            "http://example.org/Person1",
            "http://example.org/MissingPerson1",
            "http://example.org/MissingPerson2"
        }
    },
    {
        .case_name="TwoMissingThreeExisting",
        .data_file=("data/queries/common/retrieve_person_caption_data/"
                    "model-01_normal-success-cases.ttl"),
        .input_uri_seq={
            "http://example.org/Person1",
            "http://example.org/MissingPerson1",
            "http://example.org/Person2",
            "http://example.org/MissingPerson2",
            "http://example.org/Person3"
        }
    }
};

INSTANTIATE_TEST_SUITE_P(
    ,
    CommonQueries_RetrievePersonCaptionDataSeqReq_InvalidInput,
    ::testing::ValuesIn(g_missing_person_error_params),
    tools::ParamNameGen<Param>);

} // namespace test::suite_retrieve_person_caption_data_seq
