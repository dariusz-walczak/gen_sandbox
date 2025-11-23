#include "common/resource.hpp"

#include <stdexcept>
#include <spdlog/spdlog.h>

#include "common/contract.hpp"

namespace common
{

void Resource::set_uri(const std::string& uri)
{
    boost::urls::url_view url;

    try
    {
        // The URI is parsed during the url_view construction and this is the stage at which any
        //  input data validity related error may occur.
        url = uri;
    }
    catch (const std::length_error& e)
    {
        throw common_exception(
            common_exception::error_code::data_size_error,
            fmt::format(
                "The resource uri is too long:\n"
                "    {}\n", e.what()));
    }
    catch (const boost::system::system_error& e)
    {
        spdlog::debug(
            "The format of the following resource uri is invalid:\n"
            "    {}\n"
            "    {}\n", uri, e.what());

        throw common_exception(
            common_exception::error_code::data_format_error,
            fmt::format(
                "Invalid resource uri format:\n"
                "    {}\n", uri));
    }

    if (url.path().empty() || url.path() == "/")
    {
        spdlog::debug(
            "The following resource uri is invalid due to the lack of the path part:\n"
            "    {}", uri);

        throw common_exception(
            common_exception::error_code::data_format_error,
            fmt::format(
                "Invalid resource uri format (no path part):\n"
                "    {}\n", uri));
    }

    if (url.path().back() == '/')
    {
        spdlog::debug(
            "The following resource uri is invalid due to the fact that the path part ends with"
            " the '/' character:\n"
            "    {}", uri);

        throw common_exception(
            common_exception::error_code::data_format_error,
            fmt::format(
                "Invalid resource uri format (the path part ends with a slash):\n"
                "    {}\n", uri));
    }

    m_uri = url;
}
    
resource_id Resource::get_unique_id() const
{
    std::ostringstream oss;
    oss << m_uri.host() << m_uri.path();
    return oss.str();
}

std::filesystem::path Resource::get_unique_path() const
{
    // Implicit conversion from std::string to std::filesystem::path:
    return get_unique_id();
}

void Resource::print_state(std::ostream& os) const
{
    os << "uri: " << m_uri.c_str();
}

std::ostream& Resource::operator<<(std::ostream& os)
{
    os << "Resource{";
    print_state(os);
    os << "}";
    return os;
}

//  Utility Functions
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

std::vector<std::string> extract_uri_str_seq(
    const std::vector<std::shared_ptr<Resource>>& resources)
{
    ensure_resources_not_null(resources);

    std::vector<std::string> output;
    output.reserve(resources.size());

    std::ranges::transform(
        resources, std::back_inserter(output),
        [](const auto& res) { return res->get_uri_str(); });

    return output;
}

} // namespace common
