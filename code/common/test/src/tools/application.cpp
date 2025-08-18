#include "test/tools/application.hpp"

#include <boost/dll/runtime_symbol_info.hpp>

#include "common/common_exception.hpp"


namespace test::tools
{

std::filesystem::path get_program_location()
{
    std::error_code ec;
    std::filesystem::path result = boost::dll::program_location(ec);

    if (ec)
    {
        throw common::common_exception(
            common::common_exception::error_code::general_runtime_error,
            fmt::format("[{}: {}] {}", ec.category().name(), ec.value(), ec.message()));
    }

    return result;
}

} // namespace test::tools
