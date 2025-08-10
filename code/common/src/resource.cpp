#include "common/resource.hpp"

#include <stdexcept>
#include <spdlog/spdlog.h>

#include "common/common_exception.hpp"

namespace common
{

void Resource::set_iri(const std::string& iri)
{
    boost::urls::url_view url;

    try
    {
        // The IRI is parsed during the url_view construction and this is the stage at which any
        //  input data validity related error may occur.
        url = iri;
    }
    catch (const std::length_error& e)
    {
        throw common_exception(
            common_exception::error_code::data_size_error,
            fmt::format(
                "The resource iri is too long:\n"
                "    {}\n", e.what()));
    }
    catch (const boost::system::system_error& e)
    {
        spdlog::debug(
            "The format of the following resource iri is invalid:\n"
            "    {}\n"
            "    {}\n", iri, e.what());

        throw common_exception(
            common_exception::error_code::data_format_error,
            fmt::format(
                "Invalid resource iri format:\n"
                "    {}\n", iri));
    }

    if (url.path().empty() || url.path() == "/")
    {
        spdlog::debug(
            "The following resource iri is invalid due to the lack of the path part:\n"
            "    {}", iri);

        throw common_exception(
            common_exception::error_code::data_format_error,
            fmt::format(
                "Invalid resource iri format (no path part):\n"
                "    {}\n", iri));
    }

    if (url.path().back() == '/')
    {
        spdlog::debug(
            "The following resource iri is invalid due to the fact that the path part ends with"
            " the '/' character:\n"
            "    {}", iri);

        throw common_exception(
            common_exception::error_code::data_format_error,
            fmt::format(
                "Invalid resource iri format (the path part ends with a slash):\n"
                "    {}\n", iri));
    }

    m_iri = url;
}
    


resource_id Resource::get_unique_id() const
{
    std::ostringstream oss;
    oss << m_iri.host() << m_iri.path();
    return oss.str();
}

std::filesystem::path Resource::get_unique_path() const
{
    // Implicit conversion from std::string to std::filesystem::path:
    return get_unique_id();
}


std::shared_ptr<Resource> extract_resource(const data_row& row, const std::string& resource_iri_bn)
{
    auto iri_it = row.find(resource_iri_bn);

    if (iri_it == row.end())
    {
        spdlog::debug(
            "The following, expected resource IRI binding wasn't found in the processed data row:"
            " {}", resource_iri_bn);

        throw common_exception(
            common_exception::error_code::binding_not_found,
            fmt::format("The data row is missing the '{}' binding", resource_iri_bn));
    }

    // The Resource::Resource counstructor may throw common_exception:
    return std::make_shared<Resource>(iri_it->second);
}

} // namespace common
