#if !defined PERSON_ERROR_HPP
#define PERSON_ERROR_HPP

#include <cstdint>
#include <exception>
#include <optional>
#include <string>

class person_exception : public std::exception {
public:
    enum class error_code : std::uint8_t {
        uninitialized = 0,
        query_error,
        resource_not_found
    };

    person_exception(error_code code, const std::string& msg);
    person_exception(error_code code, const char* msg);

    virtual const char* what() const noexcept { return _what.c_str(); }

protected:
    error_code  _code;
    std::string _what;

    void set_what(error_code code, const char* msg);
};

#endif // !defined PERSON_ERROR_HPP
