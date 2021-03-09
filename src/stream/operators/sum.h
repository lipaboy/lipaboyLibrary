#pragma once

#include "tools.h"

namespace lipaboy_lib::stream_space {

    // NOTE: why isn't sum operator inherited from reduce operator?
	//		Because I think that such realization is faster than with lambda one. Maybe test it ?

	namespace op {

		struct sum : impl::TerminatedOperator
		{
		public:
			template <class T>
			using RetType = std::optional<T>;

		public:

			template <class TStream>
			auto apply(TStream & stream) -> RetType<typename TStream::ResultValueType>
			{
				using TResult = typename TStream::ResultValueType;
				TResult result;

				if (!stream.hasNext())
					return std::nullopt;

				result = stream.nextElem();
				while (stream.hasNext())
					result += stream.nextElem();
				return result;
			}

		};

	}

}
