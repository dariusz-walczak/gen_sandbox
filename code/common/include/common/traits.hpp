#if !defined COMMON_TRAITS_HPP
#define COMMON_TRAITS_HPP

#include <memory>
#include <type_traits>

namespace common
{

template <class T>
struct is_shared_ptr : std::false_type {};

template <class T>
struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

template <class T>
inline constexpr bool is_shared_ptr_v = is_shared_ptr<T>::value;

} // namespace common

#endif // !defined COMMON_TRAITS_HPP
