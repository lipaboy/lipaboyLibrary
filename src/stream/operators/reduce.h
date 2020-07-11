#pragma once

#include "tools.h"

#include "extra_tools/extra_tools.h"

#include <functional>
#include <algorithm>
#include <iterator>

#include <typeinfo>
#include <type_traits>

//#define _SILENCE_CXX17_RESULT_OF_DEPRECATION_WARNING
//
//#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS


namespace lipaboy_lib {

	namespace stream_space {

		namespace operators_space {

			using std::function;

			//------------------------------------------------------------------------------------------------//
			//-----------------------------------Terminated operation-----------------------------------------//
			//------------------------------------------------------------------------------------------------//

			namespace {

				template <class Accumulator>
				using GetSecondArgumentType = typename function_traits<
					lipaboy_lib::WrapBySTDFunctionType<Accumulator> >::template arg<1>::type;

				using FalseType =
					//    std::false_type;
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

				template <class F, class Accumulator>
				struct GetFirstArgumentType_ElseArg {
					using type = typename function_traits<lipaboy_lib::WrapBySTDFunctionType<F> >::template arg<0>::type;
				};

				template <class Accumulator>
				struct GetFirstArgumentType_ElseArg<FalseType, Accumulator> {
					using type = GetSecondArgumentType<Accumulator>;
				};

			}

			template <class Accumulator, class IdentityFn = FalseType >
			struct reduce : FunctorHolder<Accumulator>,
				FunctorHolder<IdentityFn>
			{
			public:
				template <class TResult, class T>
				using AccumRetType = typename std::result_of<Accumulator(TResult, T)>::type;
				using IdentityFnType = IdentityFn;
				using ArgType = typename GetFirstArgumentType_ElseArg<IdentityFnType, Accumulator>::type;
				using IdentityRetType = lipaboy_lib::enable_if_else_t<std::is_same<IdentityFn, FalseType >::value,
					ArgType, result_of_else_t<IdentityFnType(ArgType)> >;

				template <class T>
				using RetType = IdentityRetType;

				static constexpr enum OperatorMetaTypeEnum metaInfo = REDUCE;
				static constexpr bool isTerminated = true;
			public:
				reduce(IdentityFn identity, Accumulator accum)
					: FunctorHolder<Accumulator>(accum),
					FunctorHolder<IdentityFn>(identity)
				{
					//static_assert(!std::is_same<IdentityFn, std::function<int(int, int)> >::value, "0");
					//static_assert(!std::is_same<Accumulator, std::function<int(int, int)> >::value, "1");
				}
				reduce(Accumulator accum)
					: FunctorHolder<Accumulator>(accum),
					FunctorHolder<IdentityFn>(FalseType())
				{
					//static_assert(std::is_same<IdentityFn, FalseType>::value, "2");
				}

				template <class TResult_, class Arg_>
				AccumRetType<TResult_, Arg_> accum(TResult_&& result, Arg_&& arg) const {
					return FunctorHolder<Accumulator>::functor()(std::forward<TResult_>(result),
						std::forward<Arg_>(arg));
				}
				template <class Arg_>
				IdentityRetType identity(Arg_&& arg) const
				{
					if constexpr (std::is_same<IdentityFn, FalseType>::value)
						return std::forward<Arg_>(arg);
					else
						return operators_space::FunctorHolder<IdentityFn>::functor()(std::forward<Arg_>(arg));
				}

				template <class Stream_>
				auto apply(Stream_ & obj) -> IdentityRetType
				{
					using RetType = typename reduce<Accumulator, IdentityFn>::IdentityRetType;
					if (obj.hasNext()) {
						auto result = identity(obj.nextElem());
						for (; obj.hasNext(); )
							result = accum(result, obj.nextElem());
						return result;
					}
					return RetType();
				}

			};

		}

	}

}