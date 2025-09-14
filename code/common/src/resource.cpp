#include "common/resource.hpp"

#include <stdexcept>
#include <spdlog/spdlog.h>

#include "common/common_exception.hpp"

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


std::shared_ptr<Resource> extract_resource(const data_row& row, const std::string& resource_uri_bn)
{
    auto uri_it = row.find(resource_uri_bn);

    if (uri_it == row.end())
    {
        spdlog::debug(
            "The following, expected resource URI binding wasn't found in the processed data row:"
            " {}", resource_uri_bn);

        throw common_exception(
            common_exception::error_code::binding_not_found,
            fmt::format("The data row is missing the '{}' binding", resource_uri_bn));
    }

    // The Resource::Resource counstructor may throw common_exception:
    return std::make_shared<Resource>(uri_it->second);
}

} // namespace common
