#pragma once

#include "tools.h"

#include <memory>

namespace lipaboy_lib::short_stream {

	namespace operators {

		using stream_space::operators::FunctorHolder;

		//-------------------------------------------------------------------------------------//
		//--------------------------------Unterminated operation------------------------------//
		//-------------------------------------------------------------------------------------//

		template <class Predicate>
		struct filter : public FunctorHolder<Predicate>,
			public TReturnSameType
		{
			static constexpr bool isTerminated = false;
		public:
			filter(Predicate obj)
				: FunctorHolder<Predicate>(obj)
			{}

			// Opinion: difficult construction but without extra executions and computions

			template <class TSubStream>
			auto next(TSubStream& stream) -> RetType<typename TSubStream::ResultValueType> {
				// Info: I think you mustn't think about corrupting of branch predicator
				//		 because the gist of filtering is checking the conditions

				// ! calling hasNext() of current StreamType ! in order to skip unfilter elems
				auto resOpt = stream.next();
				while (resOpt != std::nullopt && false == FunctorHolder<Predicate>::functor()(resOpt.value()))
					resOpt = stream.next();
				return resOpt;
			}

			/*template <class TSubStream>
			void incrementSlider(TSubStream& stream) {
				stream.incrementSlider();
			}
*/
		};

	}

	/*using operators::filter;
	using operators::filter_impl;

	template <class TStream, class Predicate>
	struct shortening::StreamTypeExtender<TStream, filter<Predicate> > {
		template <class T>
		using remref = std::remove_reference_t<T>;

		using type = typename remref<TStream>::template ExtendedStreamType<
			remref<filter_impl<Predicate> > >;
	};*/

}

