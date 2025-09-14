#if !defined COMMON_RESOURCE_HPP
#define COMMON_RESOURCE_HPP

#include <memory>
#include <set>
#include <string>
#include <boost/url.hpp>

#include "common/redland_utils.hpp"

namespace common
{

struct Resource;

using resource_set = std::set<std::shared_ptr<Resource>>;
using resource_id = std::string;

struct Resource
{
public:
    Resource() = default;
    Resource(const std::string& uri) { set_uri(uri); }

    void set_uri(const std::string& uri);

    [[nodiscard]] const boost::urls::url_view get_uri() const { return m_uri; }
    [[nodiscard]] resource_id get_unique_id() const;
    [[nodiscard]] std::filesystem::path get_unique_path() const;

    bool operator<(const Resource& other) const { return m_uri < other.m_uri; }
    bool operator==(const Resource& other) const { return m_uri == other.m_uri; }

private:
    /** This field stores the resource URI
     *
     * @note The boost::urls::url type doesn't support IRIs, but instead supports ASCII URLs as
     *     specified by RFC 3986. This issue should be fixed either by converting an IRI to a URL
     *     or by providing an alternative IRI parser. This effort is just not important right now.
     */
    boost::urls::url m_uri;
};

/** Extract resource URI from the specified data row field
 *
 *  @throw common_exception when the resource_uri_bn binding was not found in the data row;
 *      when the uri format is invalid; */
std::shared_ptr<Resource> extract_resource(
    const data_row& row, const std::string& resource_uri_bn);

} // namespace common

#endif // !defined COMMON_RESOURCE_HPP
