#pragma once

#include "tools.h"

#include "extra_tools/extra_tools.h"

#include <functional>
#include <iterator>
#include <type_traits>
#include <optional>

namespace lipaboy_lib::stream_space {

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

	namespace operators {

		//--------------------------Reduce Operator----------------------------//
		
		template <class AccumulatorFn, class IdentityFn = shortening::FalseType>
		struct reduce :
			impl::FunctorHolder<AccumulatorFn>,
			impl::FunctorHolder<IdentityFn>,
			impl::TReturnSameType,
			impl::TerminatedOperator
		{
		public:
			reduce(AccumulatorFn&& accum, IdentityFn&& identity)
				: impl::FunctorHolder<AccumulatorFn>(accum),
				impl::FunctorHolder<IdentityFn>(identity)
			{
				// doesn't work
				static_assert(shortening::GetArgumentCount<AccumulatorFn> == 2,
					"Stream.Reduce Error: count arguments of lambda \
						function is not equal to 2.");
			}
			reduce(AccumulatorFn&& accum)
				: impl::FunctorHolder<AccumulatorFn>(accum),
				impl::FunctorHolder<IdentityFn>([]() {})
			{
				static_assert(shortening::GetArgumentCount<AccumulatorFn> == 2,
					"Stream.Reduce Error: count arguments of lambda \
						function is not equal to 2.");
			}

			impl::FunctorHolder<AccumulatorFn> accum() { return impl::FunctorHolder<AccumulatorFn>(*this); }
			impl::FunctorHolder<IdentityFn> identity() { return impl::FunctorHolder<IdentityFn>(*this); }

		};

		namespace impl {

			using std::function;
			using shortening::FalseType;
			using shortening::GetArgumentCount;
			using shortening::GetFirstArgumentType;
			using shortening::GetSecondArgumentType;

			template <class AccumulatorFn, class IdentityFn = FalseType>
			struct reduce_impl :
				FunctorHolder<AccumulatorFn>,
				FunctorHolder<IdentityFn>,
				TerminatedOperator
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
					return impl::FunctorHolder<AccumulatorFn>::functor()(std::forward<TResult_>(result),
						std::forward<Arg_>(arg));
				}

				template <class Arg_>
				AccumRetType identity(Arg_&& arg) const {
					if constexpr (std::is_same_v<IdentityFn, FalseType>)
						return AccumRetType(std::forward<Arg_>(arg));
					else
						return impl::FunctorHolder<IdentityFn>::functor()(std::forward<Arg_>(arg));
				}

				template <class Stream_>
				auto apply(Stream_& obj) -> RetType<void>
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

	}

	using operators::reduce;
	using operators::impl::reduce_impl;

	template <class TStream, class AccumulatorFn, class IdentityFn>
	struct shortening::TerminatedOperatorTypeApply<TStream, reduce<AccumulatorFn, IdentityFn> > {
		using type = operators::impl::reduce_impl<AccumulatorFn, IdentityFn>;
	};

}
