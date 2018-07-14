#pragma once

#include <type_traits>

// debugging
#include <iostream>

namespace lipaboy_lib {

using std::cout;
using std::endl;

// TODO: put off this instrument to another file
template<bool B, class T1, class T2>
struct enable_if_else {};
template<class T1, class T2>
struct enable_if_else<true, T1, T2> { typedef T1 type; };
template<class T1, class T2>
struct enable_if_else<false, T1, T2> { typedef T2 type; };

// forward kek

// it means that we forward variable of ForwardingType relative to variable of RelativeToType
template <class RelativeToType, class ForwardingType>
constexpr inline auto relativeForward(std::remove_reference_t<RelativeToType>& s,
                                      std::remove_reference_t<ForwardingType>& t) noexcept
    -> std::remove_reference_t<ForwardingType>&
{
    return static_cast<std::remove_reference_t<ForwardingType>&>(t);
}

template <class RelativeToType, class ForwardingType>
constexpr inline auto relativeForward(std::remove_reference_t<RelativeToType>&& s,
                                      std::remove_reference_t<ForwardingType>& t) noexcept
    -> std::remove_reference_t<ForwardingType>&&
{
    static_assert(!std::is_lvalue_reference<RelativeToType>::value,
                  "Can not forward an rvalue as an lvalue.");
    return static_cast<std::remove_reference_t<ForwardingType>&&>(t);
}

// another version

template <class RelativeToType, class ForwardingType>
struct RelativeForward {
static inline auto forward(std::remove_reference_t<ForwardingType>& t)
    -> std::remove_reference_t<ForwardingType>
{
    return static_cast<std::remove_reference_t<ForwardingType> >(t);
}
};

template <class RelativeToType, class ForwardingType>
struct RelativeForward<RelativeToType&, ForwardingType> {
static inline auto forward(
        std::remove_reference_t<ForwardingType>& t) noexcept
    -> std::remove_reference_t<ForwardingType>&
{
    return static_cast<std::remove_reference_t<ForwardingType>&>(t);
}
};

template <class RelativeToType, class ForwardingType>
struct RelativeForward<RelativeToType&&, ForwardingType> {
static inline auto forward(
        std::remove_reference_t<ForwardingType>& t) noexcept
    -> std::remove_reference_t<ForwardingType>&&
{
    static_assert(!std::is_lvalue_reference<RelativeToType>::value,
                  "Can not forward an rvalue as an lvalue.");
    return static_cast<std::remove_reference_t<ForwardingType>&&>(t);
}
};



}

//#endif // EXTRA_TOOLS_H
