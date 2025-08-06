#if !defined TEST_TOOLS_ERROR_HPP
#define TEST_TOOLS_ERROR_HPP

#include <exception>
#include <string>

namespace test::tools
{

class tc_error : public std::exception
{
public:
    tc_error(const char* msg) : m_what(msg) {}
    [[nodiscard]] const char* what() const noexcept override { return m_what.c_str(); }
protected:
    std::string m_what;
};

} // namespace test::tools

#endif // !defined TEST_TOOLS_ERROR_HPP
