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

		};

	}

}

