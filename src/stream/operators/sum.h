#pragma once

#include "tools.h"

namespace lipaboy_lib::stream_space {

	namespace operators {

		// Question: what's shit is it doing here, void* ? (Replace to optional is not trivial)

		template <class TInit = void*>
		struct sum : TReturnSameType, TerminatedOperator
		{
            sum(TInit init = nullptr) {
				init_ = init;
			}

			template <class TStream>
			auto apply(TStream & stream) -> typename TStream::ResultValueType
			{
				using TResult = typename TStream::ResultValueType;
				TResult result;
				if constexpr (std::is_same_v<TInit, void*>)
					result = TResult();
				else
					result = init_;
				while (stream.hasNext())
					result += stream.nextElem();
				return result;
			}

			TInit init_;
		};

	}

}
