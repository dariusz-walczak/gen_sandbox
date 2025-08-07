#if !defined TEST_COMMON_APPLICATION_HPP
#define TEST_COMMON_APPLICATION_HPP

#include <CLI/CLI.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "common/common_exception.hpp"
#include "common/spdlog_utils.hpp"

struct app_init_outcome
{
    bool exit_flag;
    int exit_code;
};

//app_init_outcome init_app(int argc, char **argv) noexcept;

namespace detail
{

app_init_outcome init_app(int argc, char** argv)
{
    // Default logger initialization. The log level can be changed using the command line option.
    const spdlog::level::level_enum default_log_level = spdlog::level::err;
    spdlog::set_default_logger(spdlog::stderr_color_mt("test logger"));
    spdlog::set_level(default_log_level);

    // The initial value of the specified_log_level variable is irrelevant as it is overridden
    //  by the CLI11_PARSE macro:
    spdlog::level::level_enum specified_log_level = spdlog::level::trace;

    {
        CLI::App app;
        add_log_level_cli_option(&app, specified_log_level, default_log_level);

        // The following try-catch block is a modification of the CLI11_PARSE macro expansion:
        try
        {
            app.parse(argc, argv);
        }
        catch (const CLI::ParseError &e)
        {
            return {true, app.exit(e)};
        }
    }

    spdlog::set_level(specified_log_level);

    return {false, 0}; // The cli parsing succeeded
}

} // namespace detail


app_init_outcome init_app(int argc, char** argv) noexcept
{
    try
    {
        return detail::init_app(argc, argv);
    }
    catch (const common_exception& e)
    {
        std::cerr << "ERROR: " << e.what() << "\n";

        return {true, 2};
    }
    catch (const std::exception& e)
    {
        std::cerr << "ERROR: An unhandled exception occurred: " << e.what() << "\n";

        return {true, 3};
    }
    catch (...)
    {
        std::cerr << "ERROR: An unhandled, unrecognized exception occurred\n";
        return {true, 4};
    }
}


#endif // !defined TEST_COMMON_APPLICATION_HPP
