#include <utility>

#include <gtest/gtest.h>

#include "test/tools/application.hpp"


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    app_init_outcome outcome = init_app(argc, argv);

    if (outcome.exit_flag)
    {
        return outcome.exit_code;
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

TEST(Sanity, ExpectTrue)
{
    EXPECT_TRUE(true);
}
