#if !defined COMMON_COMMON_EXCEPTION_HPP
#define COMMON_COMMON_EXCEPTION_HPP

#include <cstdint>
#include <exception>
#include <optional>
#include <string>


class common_exception : public std::exception {
public:
    enum class error_code : std::uint8_t {
        uninitialized = 0,
        /** Initialization of a new Redland RDF Library context has failed for unknown reasons */
        redland_initialization_failed,
        /** The Redland RDF Library has behaved unexpectedly. This error may occur due to a wrong
         *   assumption about the library's functionality, an interface change related to the
         *   library upgrade, or even a bug in the library implementation. */
        redland_unexpected_behavior,
        binding_not_found,
        /** The input data format is invalid for some reason. */
        data_format_error,
        /** The input data size is invalid (too small, or more likely too large). */
        data_size_error,
        /** The input data failed to fulfill a contract condition */
        input_contract_error
    };

    common_exception(error_code code, const std::string& msg);
    common_exception(error_code code, const char* msg);

    [[nodiscard]] error_code get_code() const noexcept { return _code; }
    [[nodiscard]] const char* what() const noexcept override { return _what.c_str(); }

protected:
    error_code  _code;
    std::string _what;

    void set_what(error_code code, const char* msg);
};

#endif // !defined COMMON_COMMON_EXCEPTION_HPP
