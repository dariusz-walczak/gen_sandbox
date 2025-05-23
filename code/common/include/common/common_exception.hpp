#if !defined COMMON__COMMON_EXCEPTION_HPP
#define COMMON__COMMON_EXCEPTION_HPP

#include <exception>
#include <optional>
#include <string>


class common_exception : public std::exception {
public:
    enum class error_code {
        uninitialized = 0,
        binding_not_found,
        data_format_error
    };

    common_exception(error_code code, const std::string& msg);
    common_exception(error_code code, const char* msg);

    virtual const char* what() const noexcept { return _what.c_str(); }

protected:
    error_code  _code;
    std::string _what;

    void set_what(error_code code, const char* msg);
};

#endif // !defined COMMON__COMMON_EXCEPTION_HPP
