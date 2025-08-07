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

struct Resource
{
public:
    Resource() = default;
    Resource(const std::string& iri) { set_iri(iri); }

    void set_iri(const std::string& iri);

    [[nodiscard]] const boost::urls::url_view get_iri() const { return m_iri; }
    [[nodiscard]] std::string get_unique_id() const;
    [[nodiscard]] std::filesystem::path get_unique_path() const;

    bool operator<(const Resource& other) const { return m_iri < other.m_iri; }

private:
    /** This field stores the resource IRI
     *
     * @note The boost::urls::url type doesn't support IRIs, but instead supports ASCII URLs as
     *     specified by RFC 3986. This issue should be fixed either by converting an IRI to a URL
     *     or by providing an alternative IRI parser. This effort is just not important right now.
     */
    boost::urls::url m_iri;
};

/** Extract resource IRI from the specified data row field
 *
 *  @throw common_exception when the resource_iri_bn binding was not found in the data row;
 *      when the iri format is invalid; */
std::shared_ptr<Resource> extract_resource(
    const data_row& row, const std::string& resource_iri_bn);

} // namespace common

#endif // !defined COMMON_RESOURCE_HPP
