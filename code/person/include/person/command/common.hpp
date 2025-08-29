#if !defined PERSON_COMMAND_COMMON_HPP
#define PERSON_COMMAND_COMMON_HPP

#include "common/file_system_utils.hpp"
#include "common/redland_utils.hpp"
#include "common/resource.hpp"

#include "person/option_parser.hpp"

namespace person
{

common::scoped_redland_ctx load_input_data(const cli_options& options);

common::input_files determine_input_paths(const cli_options& options);

}

#endif // !defined PERSON_COMMAND_COMMON_HPP

