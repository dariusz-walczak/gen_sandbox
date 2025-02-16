#include "person.hpp"

#include <cassert>


std::string extract_gender_raw(librdf_node* node) {
    assert(node);

    librdf_uri* uri = librdf_node_get_uri(node);
    std::string_view raw_value(
        reinterpret_cast<char*>(
            librdf_uri_as_string(uri)));

    if (raw_value == "http://gedcomx.org/Male") {
        return "male";
    } else if (raw_value == "http://gedcomx.org/Female") {
        return "female";
    } else {
        return "";
    }
}
