#pragma once

#include "extra_tools/extra_tools.h"

#include <functional>
#include <iterator>
#include <typeinfo>
#include <type_traits>

namespace lipaboy_lib::stream_space {

	namespace op::impl {

		//#define DEBUG_STREAM_WITH_NOISY

		using std::function;

		using lipaboy_lib::function_traits;
		using lipaboy_lib::WrapBySTDFunctionType;


		//---------------Special structs--------------//

		struct TReturnSameType {
			template <class T>
			using RetType = T;
		};

		struct FixSizeOperator {};

		struct TerminatedOperator {};

		template <class Functor>
		struct FunctorMetaType {
			using GetMetaType = Functor;
		};

		template <class Functor>
		struct FunctorHolderDirectly : FunctorMetaType<Functor> {
			using FunctorType = Functor;
			FunctorHolderDirectly(FunctorType func) : functor_(func) {}

			FunctorType functor() const { return functor_; }
			void setFunctor(FunctorType op) { functor_ = op; }
		private:
			FunctorType functor_;
		};

		// Wrap almost all the functions by std::function 
		// (except lambda with auto arguments and etc.)
		template <class Functor>
		struct FunctorHolderWrapper 
			: FunctorMetaType< WrapBySTDFunctionType<Functor> > 
		{
			using FunctorType = WrapBySTDFunctionType<Functor>;
			FunctorHolderWrapper(FunctorType func) : functor_(func) {}

			FunctorType functor() const { return functor_; }
			void setFunctor(FunctorType op) { functor_ = op; }
		private:
			FunctorType functor_;
		};

		template <>
		struct FunctorHolderWrapper< std::function<void(void)> >
		{
			FunctorHolderWrapper() {}
		};

		template <class Functor>
		struct FunctorHolderWrapperExcludeLambda 
			: FunctorMetaType< WrapBySTDFunctionExcludeLambdaType<Functor> > 
		{
			using FunctorType = WrapBySTDFunctionExcludeLambdaType<Functor>;
			FunctorHolderWrapperExcludeLambda(FunctorType func) : functor_(func) {}

			FunctorType functor() const { return functor_; }
			void setFunctor(FunctorType op) { functor_ = op; }
		private:
			FunctorType functor_;
		};

		template <class Functor>
		struct FunctorHolder
				//     : FunctorHolderWrapper<Functor>
			//: FunctorHolderDirectly<Functor>
			: FunctorHolderWrapperExcludeLambda<Functor>
		{
			using Base = FunctorHolderWrapperExcludeLambda<Functor>;
			//       : FunctorHolderWrapper<Functor>(func)
				//: FunctorHolderDirectly<Functor>(func)
			FunctorHolder(typename Base::FunctorType func) 
				: Base(func)
			{}
		};

	}


	namespace shortening {

		//---------------StreamTypeExtender---------------//

		template <class TStream, class TOperator>
		struct StreamTypeExtender {
			using type = typename std::remove_reference_t<TStream>::
				template ExtendedStreamType<std::remove_reference_t<TOperator> >;
		};

		template <class TStream, class TOperator>
		using StreamTypeExtender_t = typename StreamTypeExtender<TStream, TOperator>::type;

		template <class TStream, class TOperator>
		struct TerminatedOperatorTypeApply {
			using type = std::remove_reference_t<TOperator>;
		};

		template <class TStream, class TOperator>
		using TerminatedOperatorTypeApply_t =
			typename TerminatedOperatorTypeApply<TStream, TOperator>::type;

		//------------------------------------------------------------------//
		//-------------------------Useful aliases---------------------------//
		//------------------------------------------------------------------//

		template <class TFunction>
		using GetFirstArgumentType = typename function_traits<
			lipaboy_lib::WrapBySTDFunctionType<TFunction> >::template arg<0>::type;

		template <class TFunction>
		using GetSecondArgumentType = typename function_traits<
			lipaboy_lib::WrapBySTDFunctionType<TFunction> >::template arg<1>::type;

		template <class TFunction>
		constexpr size_t GetArgumentCount = function_traits<
			lipaboy_lib::WrapBySTDFunctionType<TFunction> >::nargs;

		// TODO: replace on std::false_type
		using FalseType =
			//std::false_type;
			std::function<void(void)>;

		//

		template <class T>
		struct result_of_else {
		};
		template <class F, class T>
		struct result_of_else<F(T)> {
			using type = typename std::invoke_result<F, T>::type;
		};
		template <class T>
		struct result_of_else<FalseType(T)>
			: std::false_type
		{};
		template <class T>
		using result_of_else_t = typename result_of_else<T>::type;

	}

}
