#if !defined COMMON_RESOURCE_HPP
#define COMMON_RESOURCE_HPP

#include <memory>
#include <ostream>
#include <set>
#include <string>

#include <boost/url.hpp>

#include "common/common_exception.hpp"
#include "common/note.hpp"
#include "common/redland_utils.hpp"

namespace common
{

//struct Note;
struct Resource;

using resource_set = std::set<std::shared_ptr<Resource>>;
using resource_id = std::string;

struct Resource
{
public:
    Resource() = default;
    Resource(const std::string& uri) { set_uri(uri); }
    virtual ~Resource() = default;

    void set_uri(const std::string& uri);

    [[nodiscard]] boost::urls::url_view get_uri() const { return m_uri; }
    [[nodiscard]] std::string get_uri_str() const { return m_uri.c_str(); }
    [[nodiscard]] resource_id get_unique_id() const;
    [[nodiscard]] std::filesystem::path get_unique_path() const;
    [[nodiscard]] virtual std::string get_caption() const { return ""; }

    bool operator<(const Resource& other) const { return m_uri < other.m_uri; }
    bool operator==(const Resource& other) const { return m_uri == other.m_uri; }
    std::ostream& operator<<(std::ostream& os);

    void add_note(Note note) { m_notes.emplace_back(std::move(note)); }
    [[nodiscard]] const auto& notes() const { return m_notes; }
    [[nodiscard]] auto& notes() { return m_notes; }

    virtual void print_state(std::ostream& os) const;

private:
    /** This field stores the resource URI
     *
     * @note The boost::urls::url type doesn't support IRIs, but instead supports ASCII URLs as
     *     specified by RFC 3986. This issue should be fixed either by converting an IRI to a URL
     *     or by providing an alternative IRI parser. This effort is just not important right now.
     */
    boost::urls::url m_uri;
    std::vector<Note> m_notes;
};

/** Extract resource URI from the specified data row field
 *
 *  @throw common_exception when the resource_uri_bn binding was not found in the data row;
 *      when the uri format is invalid; */
template<typename ResourceType>
std::shared_ptr<ResourceType> extract_resource(
    const data_row& row, const std::string& resource_uri_bn)
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
    return std::make_shared<ResourceType>(uri_it->second);
}

/** @brief Extract sequence of URI strings from a sequence of resources
 *
 *  @param resources the input resource sequence
 *  @return sequence of URI strings */
std::vector<std::string> extract_uri_str_seq(
    const std::vector<std::shared_ptr<Resource>>& resources);

} // namespace common

#endif // !defined COMMON_RESOURCE_HPP
