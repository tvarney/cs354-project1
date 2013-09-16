
#ifndef CS354_GENERIC_STRING_HPP
#define CS354_GENERIC_STRING_HPP

#include <string>

/* It would be better to do this with */
template <typename CharT, typename Traits, typename Alloc>
bool operator==(const std::basic_string<CharT, Traits, Alloc> &lhs,
                const std::basic_string<CharT, Traits, Alloc> &rhs)
{
    return (lhs.compare(rhs) == 0);
}

#endif
