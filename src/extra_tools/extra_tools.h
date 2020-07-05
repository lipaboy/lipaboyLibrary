#pragma once

#include <type_traits>
#include <tuple>
#include <functional>
#include <memory>

// debugging
#include <iostream>

namespace lipaboy_lib {

using std::cout;
using std::endl;


// PLAN:
//
// TODO: put it into extra namespace


template <class TWord>
inline constexpr TWord setBitsFromStart(size_t bitsCount) {
	return (bitsCount <= 0) ? TWord(0)
		: (bitsCount <= 1) ? TWord(1)
		: (TWord(1) << (bitsCount - 1)) | setBitsFromStart<TWord>(bitsCount - 1);
}


// TODO: it is not a "enable.." logic. It is like choosing the types relative to condition. 
//		 Refactor it.
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


//namespace {

template<typename T>
struct lambda_function_traits
{};

template<typename Ret, typename Class, typename... Args>
struct lambda_function_traits<Ret(Class::*)(Args...) const>
{
    static const bool is_std_function = false;
    static const size_t nargs = sizeof...(Args);
    using ResultType = Ret;

    template <size_t i>
    struct arg
    {
        using type = typename std::tuple_element<i, std::tuple<Args...> >::type;
    };
};

template<typename T>
struct lambda_function_traits2
        : lambda_function_traits<decltype(&T::operator())>
{};

//}

template<typename T>
struct function_traits
//        : lambda_function_traits<decltype(&T::operator())>
{};

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
struct function_traits<R(Args...)>
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
struct WrapLambdaBySTDFunction
{
    using type = void;
};

template<typename Ret, typename Class, typename... Args>
struct WrapLambdaBySTDFunction<Ret(Class::*)(Args...) const>
{
    using type = std::function<Ret(Args...)>;
};

template<typename T>
struct is_lambda : std::true_type
{};

template<typename Ret, typename Class, typename... Args>
struct is_lambda<Ret(Class::*)(Args...) const>
	: std::false_type
{};

}

template <typename F>
struct WrapBySTDFunction {
    using type =
        typename WrapLambdaBySTDFunction<decltype(&F::operator())>::type;
//        void;
};

template <typename R, typename ...Args>
struct WrapBySTDFunction< R(Args...) > {
    using type = std::function< R(Args...) >;
};

template <typename R, typename ...Args>
struct WrapBySTDFunction< R(*)(Args...) > {
    using type = std::function< R(Args...) >;
};

template <typename R, typename ...Args>
struct WrapBySTDFunction<std::function<R(Args...)> > {
    using type = std::function<R(Args...)>;
};

template <typename T>
using WrapBySTDFunctionType = typename WrapBySTDFunction<T>::type;

//--------------------Wrap by std::function exclude lambda--------------------//

template<typename F>
struct NoWrapLambdaBySTDFunction
{
	using type = F;
};

template<typename Ret, typename Class, typename... Args>
struct NoWrapLambdaBySTDFunction<Ret(Class::*)(Args...) const>
{
	using type = Ret(Class::*)(Args...);
};

namespace {

	// INFO: this crutch must be for specific lambdas which have auto specificator in signature
	//		or at return type.
	template <class T>
	using enable_getting_operator_type_if = 
		decltype(&enable_if_else_t<std::is_invocable_v<T>, 
			T, std::function<int(int)> 
		>::operator());

}

template <class F>
struct WrapBySTDFunctionExcludeLambda {
	template <class F>
	using NoWrapLambda = typename NoWrapLambdaBySTDFunction<F>::type;

	using type = 
		enable_if_else_t<std::is_invocable_v<F> //&& !is_lambda<F>::value
		, 
			NoWrapLambda< enable_getting_operator_type_if<F> >,
			F
		>;
};

template <typename R, typename ...Args>
struct WrapBySTDFunctionExcludeLambda< R(Args...) > {
	using type = std::function< R(Args...) >;
};

template <typename R, typename ...Args>
struct WrapBySTDFunctionExcludeLambda< R(*)(Args...) > {
	using type = std::function< R(Args...) >;
};

template <typename R, typename ...Args>
struct WrapBySTDFunctionExcludeLambda< std::function<R(Args...)> > {
	using type = std::function<R(Args...)>;
};

template <typename T>
using WrapBySTDFunctionExcludeLambdaType = typename WrapBySTDFunctionExcludeLambda<T>::type;

}

//#endif // EXTRA_TOOLS_H
