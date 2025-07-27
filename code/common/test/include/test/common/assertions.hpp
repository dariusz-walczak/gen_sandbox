#if !defined TEST_COMMON_ASSERTIONS_HPP
#define TEST_COMMON_ASSERTIONS_HPP

#include <gtest/gtest.h>
#include <gtest/internal/gtest-port.h>

#define EXPECT_THROW_WITH_CODE(statement, expected_exception, expected_code)                      \
    do                                                                                            \
{                                                                                                 \
    try                                                                                           \
    {                                                                                             \
        statement;                                                                                \
        FAIL() << "Expected: `" #statement "` throws an exception of type `" #expected_exception  \
            "`.\nActual: `" #statement "` throws nothing.";                                       \
    }                                                                                             \
    catch (const expected_exception& ex)                                                          \
    {                                                                                             \
        EXPECT_EQ(ex.get_code(), expected_code) << "Invalid " #expected_exception " error code";  \
    }                                                                                             \
    catch (const std::exception& ex)                                                              \
    {                                                                                             \
        FAIL() << "Expected: `" #statement "` throws an exception of type `" #expected_exception  \
            "`.\nActual: `" #statement "` throws an `" << GTEST_EXCEPTION_TYPE_(ex) <<            \
            "` exception.";                                                                       \
    }                                                                                             \
    catch (...) {                                                                                 \
        FAIL() << "Expected: `" #statement "` throws an exception of type `" #expected_exception  \
            "`.\nActual: `" #statement "` throws a different exception.";                         \
    }                                                                                             \
} while (0)


#endif // !defined TEST_COMMON_ASSERTIONS_HPP
