#ifndef EXTRA_TOOLS_H
#define EXTRA_TOOLS_H

namespace lipaboy_lib {

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
constexpr inline std::remove_reference_t<ForwardingType>& relativeForward(
        std::remove_reference_t<RelativeToType>& s,
        std::remove_reference_t<ForwardingType>& t) noexcept
{
    return static_cast<std::remove_reference_t<ForwardingType>&>(t);
}

template <class Stream_, class RelativeTo>
constexpr inline std::remove_reference_t<RelativeTo>&& relativeForward(
        std::remove_reference_t<Stream_>&& s,
        std::remove_reference_t<RelativeTo>& t) noexcept
{
    static_assert(!std::is_lvalue_reference<Stream_>::value,
                  "Can not forward an rvalue as an lvalue.");
    return static_cast<std::remove_reference_t<RelativeTo>&&>(t);
}

}

#endif // EXTRA_TOOLS_H
