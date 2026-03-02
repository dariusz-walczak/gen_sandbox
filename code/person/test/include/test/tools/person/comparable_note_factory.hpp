#if !defined TEST_TOOLS_PERSON_COMPARABLE_NOTE_FACTORY_HPP
#define TEST_TOOLS_PERSON_COMPARABLE_NOTE_FACTORY_HPP

#include "test/tools/note.hpp"

namespace test::tools::person
{

ComparableNote create_inferred_partner_comparable_note(
    const char* partner_uri, const char* partner_caption=nullptr);

ComparableNote create_invalid_inferred_partner_comparable_note(const char* partner_uri);
ComparableNote create_invalid_stated_partner_comparable_note(const char* partner_uri);

ComparableNote create_stubbed_parent_comparable_note(const tools::ComparableResource& parent);
ComparableNote create_untyped_parent_comparable_note(const tools::ComparableResource& parent);
ComparableNote create_mistyped_parent_comparable_note(const tools::ComparableResource& parent);
ComparableNote create_unknown_parent_gender_comparable_note(
    const tools::ComparableResource& parent);
ComparableNote create_invalid_parent_gender_comparable_note(
    const tools::ComparableResource& parent);

} // namespace test::tools::person

#endif // TEST_TOOLS_PERSON_COMPARABLE_NOTE_FACTORY_HPP
