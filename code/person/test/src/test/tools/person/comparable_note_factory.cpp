#include "test/tools/person/comparable_note_factory.hpp"

#include "common/note.hpp"
#include <fmt/format.h>

#include "person/queries/details.hpp"

namespace test::tools::person
{

ComparableNote create_invalid_inferred_partner_comparable_note(const char* partner_uri)
{
    return {
        .type=common::Note::Type::Info,
        .id=std::string(::person::k_invalid_inferred_partner_note_id),
        .vars={
            tools::ComparableVariable{
                .name="partner",
                .value={tools::ComparableResource{partner_uri, ""}}
            }
        },
        .diagnostic_text=fmt::format("Invalid inferred partner: {}", partner_uri)
    };
}

ComparableNote create_invalid_stated_partner_comparable_note(const char* partner_uri)
{
    return {
        .type=common::Note::Type::Warning,
        .id=std::string(::person::k_invalid_stated_partner_note_id),
        .vars={
            tools::ComparableVariable{
                .name="partner",
                .value={tools::ComparableResource{partner_uri, ""}}
            }
        },
        .diagnostic_text=fmt::format("Invalid stated partner: {}", partner_uri)
    };
}

} // namespace test::tools::person
