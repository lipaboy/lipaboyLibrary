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

}

template <typename F>
struct WrapBySTDFunction {
    using type =
        typename WrapLambdaBySTDFunction<decltype(&F::operator())>::type;
//        void;
};

template<typename R, typename ...Args>
struct WrapBySTDFunction< R(Args...) > {
    using type = std::function< R(Args...) >;
};

template<typename R, typename ...Args>
struct WrapBySTDFunction< R(*)(Args...) > {
    using type = std::function< R(Args...) >;
};

template<typename R, typename ...Args>
struct WrapBySTDFunction<std::function<R(Args...)> > {
    using type = std::function<R(Args...)>;
};

template <typename T>
using WrapBySTDFunctionType = typename WrapBySTDFunction<T>::type;


//-----------------------------------------------------------------------//
//---------------------PRODUCING ITERATOR BY FUNCTION--------------------//
//-----------------------------------------------------------------------//

template <class T>
class ProducingIterator {
public:
	using GeneratorType = std::function<T(void)>;
	using value_type = T;
	using CurrentValueType = value_type;
	using CurrentValueTypePtr = std::shared_ptr<CurrentValueType>;
	using reference = T & ;
	using const_reference = const reference;
	using pointer = T * ;
	using const_pointer = const pointer;
	using iterator_category = std::input_iterator_tag;
		// It is type of measuring the distance between iterators (nothing else).
		// It is commented because ProducingIterator is not a real iterator. You can't count the distance
		// between iterators
	//using difference_type = std::ptrdiff_t;		

public:
	ProducingIterator(GeneratorType gen) 
		: generator_(gen), pCurrentElem_(std::make_shared<CurrentValueType>(gen()))
	{}
	ProducingIterator(ProducingIterator const & obj)
		: generator_(obj.generator_),
		pCurrentElem_(obj.pCurrentElem_)
	{}

	const_reference operator*() { return *pCurrentElem_; }
	const_pointer operator->() { return &(*pCurrentElem_); }

	// Note: not strong condition (maybe add counter to distinguishing the different iterators)
	bool operator== (ProducingIterator & other) {
		return generator_.target<T(void)>() == other.generator_.target<T(void)>()
			&& pCurrentElem_ == other.pCurrentElem_
			&& *pCurrentElem_ == *(other.pCurrentElem_);
	}
	bool operator!= (ProducingIterator & other) { return !((*this) == other); }

	ProducingIterator operator++() {
		*pCurrentElem_ = generator_();
		return *this;
	}
	// Info: Return type is void because you cannot return previous iterator.
	//		 You cannot return previous iterator because pCurrentElem_ of different copies [iterators]
	//		 point to the same variable. If we don't have pointer to current elem then
	//		 we must have storage it directly (as a field). But copy constructor of iterator will be expensive.
	void operator++(int) {
		*pCurrentElem_ = generator_();
	}

private:
	GeneratorType generator_;
	CurrentValueTypePtr pCurrentElem_;
};

template <class T>
class InitializerListIterator {
public:
	using value_type = T;
	using ContainerType = std::vector<value_type>;
	using ContainerTypePtr = std::shared_ptr<ContainerType>;
	using Subiterator = typename ContainerType::iterator;
	using reference = T & ;
	using const_reference = const reference;
	using pointer = T * ;
	using const_pointer = const pointer;
	using iterator_category = std::input_iterator_tag;
	using difference_type = std::ptrdiff_t;		// type of measuring the distance between iterators (nothing else)

public:
	InitializerListIterator(std::initializer_list<T> init)
		: pContainer_(std::make_shared<ContainerType>(init))
	{
		subiter_ = pContainer_->begin();
	}
	InitializerListIterator(InitializerListIterator const & obj)
		: pContainer_(obj.pContainer_),
		subiter_(obj.subiter_)
	{}

	const_reference operator*() { return *subiter_; }
	const_pointer operator->() { return &(*subiter_); }

	// Not strong condition
	bool operator== (InitializerListIterator const & other) const {
		return pContainer_ == other.pContainer_
			&& subiter_ == other.subiter_;
	}
	bool operator!= (InitializerListIterator const & other) const { return !((*this) == other); }

	InitializerListIterator operator++() {
		++subiter_;
		return *this;
	}
	InitializerListIterator operator++(int) {
		InitializerListIterator prev = *this;
		++subiter_;
		return prev;
	}

private:
	ContainerTypePtr pContainer_;
	Subiterator subiter_;
	// maybe I need to add the counter for distinguishing the iterators
};

}

//#endif // EXTRA_TOOLS_H
