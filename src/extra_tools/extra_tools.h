#pragma once

#include <type_traits>
#include <tuple>
#include <functional>

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

template<bool B, class T1, class T2>
using enable_if_else_t = typename enable_if_else<B, T1, T2>::type;

//-------------------------------------------------------------------//
//-------------------------Relative Forward--------------------------//
//-------------------------------------------------------------------//

// Main version

// it means that we forward variable of ForwardingType relative to variable of RelativeToType
template <class RelativeToType, class ForwardingType>
struct RelativeForward {
//static inline auto forward(std::remove_reference_t<ForwardingType>& t)
//    -> std::remove_reference_t<ForwardingType>
//{
//    return static_cast<std::remove_reference_t<ForwardingType> >(t);
//}
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

// Useless version

namespace useless {

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

}


//-------------------------------------------------------------------//
//-------------------------FUNCTION TRAITS---------------------------//
//-------------------------------------------------------------------//


template<typename T>
struct function_traits;

template<typename R, typename ...Args>
struct function_traits<std::function<R(Args...)> >
{
    static const bool is_std_function = true;
    static const size_t nargs = sizeof...(Args);
    using ResultType = R;

    template <size_t i>
    struct arg
    {
        using type = typename std::tuple_element<i, std::tuple<Args...> >::type;
    };
};

template<typename R, typename ...Args>
struct function_traits<R(Args...) >
{
    static const bool is_std_function = false;
    static const size_t nargs = sizeof...(Args);
    using ResultType = R;

    template <size_t i>
    struct arg
    {
        using type = typename std::tuple_element<i, std::tuple<Args...> >::type;
    };
};


//------------------------------------------------------------------------------//
//-----------------Making std::function wrapper around any function-------------//
//------------------------------------------------------------------------------//


namespace {

template<typename T>
struct memfun_type
{
    using type = void;
};

template<typename Ret, typename Class, typename... Args>
struct memfun_type<Ret(Class::*)(Args...) const>
{
    using type = std::function<Ret(Args...)>;
};

}

template <typename F>
struct WrapBySTDFunction {
    using type =
        typename memfun_type<decltype(&F::operator())>::type;
//        void;
};

template<typename R, typename ...Args>
struct WrapBySTDFunction< R(Args...) > {
    using type = std::function< R(Args...) >;
};

template<typename R, typename ...Args>
struct WrapBySTDFunction<std::function<R(Args...)> > {
    using type = std::function<R(Args...)>;
};

//template<typename Ret, typename Class, typename... Args>
//struct WrapBySTDFunction<Ret(Class::*)(Args...) const> {
//    using type = std::function<Ret(Args...)>;
//};

template <typename T>
using WrapBySTDFunctionType = typename WrapBySTDFunction<T>::type;



template<typename F>
typename memfun_type<decltype(&F::operator())>::type
FFL(F const &func)
{ // Function from lambda !
//    cout << typeid(F).name() << endl;
//    cout << typeid(typename memfun_type<decltype(&F::operator())>::type).name() << endl;
//    cout << function_traits<typename memfun_type<decltype(&F::operator())>::type>::is_std_function << endl;
    return func;
}

//template <typename T>
//struct identity
//{
//  typedef T type;
//};

//template <typename... T>
//decltype(auto) FFL2(typename identity<std::function<void(T...)>>::type f) {
//  return f;
//}

}

//#endif // EXTRA_TOOLS_H
