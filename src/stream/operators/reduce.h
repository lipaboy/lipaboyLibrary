#pragma once

#include "tools.h"

#include "extra_tools/extra_tools.h"

#include <functional>
#include <algorithm>
#include <iterator>
#include <memory>
#include <typeinfo>
#include <type_traits>
#include <optional>


namespace lipaboy_lib {

	namespace stream_space {

		// Contract rules :
		// 1) Reduce operator must be initialized with start value because
		//		impossible to predict if the stream contains elements or not.
		// 2) IdentityFn - type of function. Necessary for correct initializing the
		//		accum variable. It takes first stream element and pass through
		//		identity function. For example, accumulator and argument types
		//		could be different.

		//------------------------------------------------------------------------------------------------//
		//-----------------------------------Terminated operation-----------------------------------------//
		//------------------------------------------------------------------------------------------------//

		namespace shortening {

			//------------------------------------------------------------------//
			//-------------------------Useful aliases---------------------------//
			//------------------------------------------------------------------//

			template <class AccumulatorFn>
			using GetFirstArgumentType = typename function_traits<
				lipaboy_lib::WrapBySTDFunctionType<AccumulatorFn> >::template arg<0>::type;

			template <class AccumulatorFn>
			using GetSecondArgumentType = typename function_traits<
				lipaboy_lib::WrapBySTDFunctionType<AccumulatorFn> >::template arg<1>::type;

			template <class AccumulatorFn>
			constexpr size_t GetArgumentCount = typename function_traits<
				lipaboy_lib::WrapBySTDFunctionType<AccumulatorFn> >::nargs;

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

			//

			template <class F, class AccumulatorFn>
			struct GetFirstArgumentType_ElseArg {
				using type = typename function_traits<lipaboy_lib::WrapBySTDFunctionType<F> >
					::template arg<0>::type;
			};

			template <class AccumulatorFn>
			struct GetFirstArgumentType_ElseArg<FalseType, AccumulatorFn> {
				using type = GetSecondArgumentType<AccumulatorFn>;
			};

		}

		namespace operators {

			using std::function;
			using namespace shortening;

			//--------------------------Reduce Operator----------------------------//

			template <class AccumulatorFn, class IdentityFn = FalseType>
			struct reduce :
				FunctorHolder<AccumulatorFn>,
				FunctorHolder<IdentityFn>,
				TReturnSameType
			{
			public:
				static constexpr enum OperatorMetaTypeEnum metaInfo = REDUCE;
				static constexpr bool isTerminated = true;

			public:

				reduce(AccumulatorFn&& accum, IdentityFn&& identity)
					: FunctorHolder<AccumulatorFn>(accum),
					FunctorHolder<IdentityFn>(identity)
				{
					// doesn't work
					static_assert(GetArgumentCount<AccumulatorFn> == 2,
						"Stream.Reduce Error: count arguments of lambda \
							function is not equal to 2.");
				}
				reduce(AccumulatorFn&& accum)
					: FunctorHolder<AccumulatorFn>(accum),
					FunctorHolder<IdentityFn>([]() {})
				{
					static_assert(GetArgumentCount<AccumulatorFn> == 2,
						"Stream.Reduce Error: count arguments of lambda \
							function is not equal to 2.");
				}

				FunctorHolder<AccumulatorFn> accum() { return FunctorHolder<AccumulatorFn>(*this); }
				FunctorHolder<IdentityFn> identity() { return FunctorHolder<IdentityFn>(*this); }

			};

			template <class AccumulatorFn, class IdentityFn = FalseType>
			struct reduce_impl : 
				FunctorHolder<AccumulatorFn>,
				FunctorHolder<IdentityFn>
			{
			public:
				using ArgType = GetSecondArgumentType<AccumulatorFn>;
				using AccumRetType = 
					std::remove_reference_t<
						std::invoke_result_t <
							AccumulatorFn, GetFirstArgumentType<AccumulatorFn>, ArgType
						>
					>;

				template <class T>
				using RetType = std::optional<AccumRetType>;

				static constexpr enum OperatorMetaTypeEnum metaInfo = REDUCE;
				static constexpr bool isTerminated = true;
			public:
				reduce_impl(reduce<AccumulatorFn, IdentityFn> reduceObj) 
					: FunctorHolder<AccumulatorFn>(reduceObj.accum().functor()),
					FunctorHolder<IdentityFn>(reduceObj.identity().functor())
				{}
				reduce_impl(AccumulatorFn&& accum, IdentityFn&& identity)
					: FunctorHolder<AccumulatorFn>(accum),
					FunctorHolder<IdentityFn>(identity)
				{}
				reduce_impl(AccumulatorFn&& accum)
					: FunctorHolder<AccumulatorFn>(accum),
					FunctorHolder<IdentityFn>([]() {})
				{}

				template <class TResult_, class Arg_>
				AccumRetType accum(TResult_&& result, Arg_&& arg) const {
					return FunctorHolder<AccumulatorFn>::functor()(std::forward<TResult_>(result),
						std::forward<Arg_>(arg));
				}

				template <class Arg_>
				AccumRetType identity(Arg_&& arg) const {
					if constexpr (std::is_same_v<IdentityFn, FalseType>)
						return AccumRetType(std::forward<Arg_>(arg));
					else
						return operators::FunctorHolder<IdentityFn>::functor()(std::forward<Arg_>(arg));
				}

				template <class Stream_>
				auto apply(Stream_ & obj) -> RetType<void>
				{
					if (!obj.hasNext())
						return std::nullopt;
					AccumRetType result = 
                        this->template identity<ArgType>(obj.nextElem());
					for (; obj.hasNext(); )
						result = accum(result, obj.nextElem());
					return result;
				}

			};

		}

		using operators::reduce;
		using operators::reduce_impl;

		template <class TStream, class AccumulatorFn, class IdentityFn>
		struct shortening::TerminatedOperatorTypeApply<TStream, reduce<AccumulatorFn, IdentityFn> > {
			using type = operators::reduce_impl<AccumulatorFn, IdentityFn>;
		};

	}

}
