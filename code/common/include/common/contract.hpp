#if !defined COMMON_CONTRACT_HPP
#define COMMON_CONTRACT_HPP

#include <memory>
#include <vector>


namespace common
{

struct Resource;

/** @brief Ensure that the provided sequence of resource pointers doesn't include a null pointer.
 *
 * @param[in] resources The input sequence to be checked
 *
 * @throws common::common_exception (input_contract_error) when at least one of the input sequence
 *     elements is a null pointer */
void ensure_resources_not_null(const std::vector<std::shared_ptr<Resource>>& resources);

} // namespace common

#endif // !defined COMMON_CONTRACT_HPP
