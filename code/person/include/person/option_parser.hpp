#if !defined PERSON__OPTION_PARSER_HPP
#define PERSON__OPTION_PARSER_HPP

#include <vector>
#include <string>

#include <CLI/CLI.hpp>


struct app_options
{
    std::vector<std::string> input_paths;
    std::string base_path_raw;

    struct details
    {
        std::string person_id;
    } details_cmd;
};

void init_option_parser(CLI::App& parser, app_options& result);

#endif // !defined PERSON__OPTION_PARSER_HPP
