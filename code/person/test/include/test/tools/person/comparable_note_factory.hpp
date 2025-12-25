#if !defined TEST_TOOLS_PERSON_COMPARABLE_NOTE_FACTORY_HPP
#define TEST_TOOLS_PERSON_COMPARABLE_NOTE_FACTORY_HPP

#include "test/tools/note.hpp"

namespace test::tools::person
{

ComparableNote create_invalid_inferred_partner_comparable_note(const char* partner_uri);
ComparableNote create_invalid_stated_partner_comparable_note(const char* partner_uri);

} // namespace test::tools::person

#endif // TEST_TOOLS_PERSON_COMPARABLE_NOTE_FACTORY_HPP
