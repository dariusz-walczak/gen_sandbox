
#include <memory>
#include <string>
#include <utility>

#include <CLI/CLI.hpp>
#include <gtest/gtest.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "common/common_exception.hpp"
#include "common/spdlog_utils.hpp"
#include "common/redland_utils.hpp"

struct init_outcome
{
    bool exit_flag;
    int exit_code;
};

init_outcome init_app(int argc, char **argv)
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

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    try
    {
        init_outcome outcome = init_app(argc, argv);

        if (outcome.exit_flag)
        {
            return outcome.exit_code;
        }
    }
    catch (const common_exception& e)
    {
        std::cerr << "ERROR: " << e.what() << "\n";

        return 2;
    }
    catch (const std::exception& e)
    {
        std::cerr << "ERROR: An unhandled exception occurred: " << e.what() << "\n";

        return 3;
    }
    catch (...)
    {
        std::cerr << "ERROR: An unhandled, unrecognized exception occurred\n";
        return 4;
    }

    const auto ret = RUN_ALL_TESTS();

    /* Workaround for an unexpected and not fully understood behavior of the spdlog library:
     *  For some reason the `spdlog::set_level` call doesn't affect the logs produced by the
     *  statically linked, common library, even if it should.
     * Facts:
     * + spdlog 1.12.0
     * + The default logger instance accessed by the logging statements used in the common library
     *   is the same as the one accessed from within the test application;
     * + In fact, the minimal workaround is to invoke spdlog::default_logger_raw() from within the
     *   application, e.g.: `std::ignore = spdlog::default_logger_raw();`
     * + The interesting fact is that the default_logger_raw call can even occur as the last call
     *   in the main function (one that follows the RUN_ALL_TESTS() macro and precedes the return
     *   statement All the logs would look normal and in place (so it seems to influence something
     *   at the compilation stage, maybe something is optimized out?);
     * + An alternative is to log something from the application after the logger initialization,
     *   e.g.: `spdlog::info("Unleash the logs!");
     * + This issue didn't occur in the gen_person application, but the difference is that this
     *   application was producing its logs, contrary to the test application.
     *
     * There appears to be a space for further investigation, but I have put it aside for now, as
     *  the workaround works, and this is a test application only. I may revisit it later.
     */
    std::ignore = spdlog::default_logger_raw();

    return ret;
}
