#if !defined COMMON_COMPARATORS_HPP
#define COMMON_COMPARATORS_HPP

namespace common
{

/** @brief Pointer dereferencing, null-first strict weak comparator
 *
 * The comparator satisfies the C++ Standard Library Compare requirements */
template<class Ptr>
struct PointeeLessNullFirst
{
    bool operator()(const Ptr& first, const Ptr& second) const
    {
        if (!first) // first is null, second is null or non-null
        {
            return static_cast<bool>(second); // null < non-null; equal if both null
        }

        if (!second) // first is non-null, second is null
        {
            return false; // non-null > null
        }

        // both are non-null
        return *first < *second;
    }
};

} // namespace common

#endif // !defined COMMON_COMPARATORS_HPP
