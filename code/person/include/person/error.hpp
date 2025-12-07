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
        resource_not_found,
        /** @brief Multiple resources were found when at most one was expected
         *
         *  @par Use Cases
         *      Throw when a SPARQL query expected to return a single resource for a given context
         *      instead returns multiple matching resources.
         *
         *  @par Typical Causes
         *      This error code typically indicates an error in the input data.
         *
         *  @par Expected Message Format
         *     `Too Many Resources: found <count> resources when <condition>` */
        multiple_resources_found,
        /** @brief Input contract violated: argument(s) fail the function’s preconditions
         *
         *  @par Use Cases
         *      Throw when any function argument violates documented preconditions (type, domain,
         *      range, invariants, or cross-param relations)
         *
         *  @par Expected Message Format
         *     `Precondition failure: <param>=<value> must satisfy <condition>` */
        input_contract_error,
        /** @brief Internal contract violated: runtime assumption failed
         *
         *  @par Use Cases
         *      Throw when code-level assumptions about internal state, data shape, type set, or
         *      or interaction sequencing prove false at runtime.
         *
         *  @par Example
         *      An aliased std::variant holds unexpected alternative.
         *
         *  @par Typical Causes
         *      This error code typically indicates an implementation issue, an outdated
         *      assumption, or a breaking interface change.
         *
         *  @par Expected Message Format
         *     `Assumption failure: expected <condition>; observed <actual>` */
        internal_contract_error
    };

    person_exception();
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
