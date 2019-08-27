#pragma once

#include "tools.h"

#include "extra_tools/extra_tools.h"

#include <functional>
#include <algorithm>
#include <iterator>
#include <memory>
#include <typeinfo>
#include <type_traits>

namespace lipaboy_lib {

	namespace stream {

		namespace operators {

			using std::function;

			// Contract rules :
			// 1) Reduce operator must be initialized with start value because
			//		impossible to predict if the stream contains elements or not.

			// THINK ABOUT : remove std::shared_ptr or not ??
			// Argument 1: maybe less copying of init value
			// Another varic: you can add specialization of template class where
			//		no members to store init value. Use function-initializer.
			// Default constructor problem :
			//		a) integral types - unspecific behaviour with different compilers
			//		b) not every type has default constructor

			//------------------------------------------------------------------------------------------------//
			//-----------------------------------Terminated operation-----------------------------------------//
			//------------------------------------------------------------------------------------------------//

			namespace {

				template <class Accumulator>
				using GetFirstArgumentType = typename function_traits<
					lipaboy_lib::WrapBySTDFunctionType<Accumulator> >::template arg<0>::type;

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
					using type = typename std::result_of<F(T)>::type;
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

			template <class Accumulator>
			struct reduce : FunctorHolder<Accumulator>
			{
			public:
				using ArgType = GetSecondArgumentType<Accumulator>;
				using AccumRetType = std::remove_reference_t<GetFirstArgumentType<Accumulator> >;

				template <class T>
				using RetType = AccumRetType;

				static constexpr enum OperatorMetaTypeEnum metaInfo = REDUCE;
				static constexpr bool isTerminated = true;
			public:
				reduce(Accumulator&& accum, AccumRetType init)
					: FunctorHolder<Accumulator>(accum),
					pInit(std::make_shared<AccumRetType>(std::move(init)))
				{}

				template <class TResult_, class Arg_>
				AccumRetType accum(TResult_&& result, Arg_&& arg) const {
					return FunctorHolder<Accumulator>::functor()(std::forward<TResult_>(result),
						std::forward<Arg_>(arg));
				}

				template <class Stream_>
				auto apply(Stream_ & obj) -> AccumRetType
				{
					auto result = std::move(*pInit);
					for (; obj.hasNext(); )
						result = accum(result, obj.nextElem());
					return result;
				}

			private:
				std::shared_ptr<AccumRetType> pInit;

			};

		}

	}

}