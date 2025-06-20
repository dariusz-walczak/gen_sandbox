#if !defined PERSON_COMMAND_COMMON_HPP
#define PERSON_COMMAND_COMMON_HPP

#include "common/redland_utils.hpp"
#include "person/option_parser.hpp"

scoped_redland_ctx load_input_data(const app_options& options);

#endif // !defined PERSON_COMMAND_COMMON_HPP

