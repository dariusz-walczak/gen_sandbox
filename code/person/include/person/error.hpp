#if !defined PERSON_ERROR_HPP
#define PERSON_ERROR_HPP

#include <cstdint>
#include <exception>
#include <optional>
#include <string>

namespace person
{

class person_exception : public std::exception
{
public:
    enum class error_code : std::uint8_t {
        uninitialized = 0,
        query_error,
        resource_not_found,
        /** @brief Multiple resources were found when at most one was expected
         *
         *  Throw when a SPARQL query expected to return a single resource for a given context
         *   instead returns multiple matching resources. This error code typically indicates
         *   an error in the input data.
         *
         *  @par Expected message format
         *     `Too Many Resources: found <count> resources when <condition>` */
        multiple_resources_found,
        /** @brief Input contract violated: argument(s) fail the function’s preconditions
         *
         *  Throw when any function argument violates documented preconditions (type, domain,
         *   range, invariants, or cross-param relations)
         *
         *  @par Expected message format
         *     `Precondition failure: <param>=<value> must satisfy <condition>` */
        input_contract_error
    };

    person_exception(error_code code, const std::string& msg);
    person_exception(error_code code, const char* msg);

    [[nodiscard]] error_code get_code() const noexcept { return _code; }
    [[nodiscard]] const char* what() const noexcept override { return _what.c_str(); }

protected:
    error_code  _code;
    std::string _what;

    void set_what(error_code code, const char* msg);
};

} // namespace person

#endif // !defined PERSON_ERROR_HPP
