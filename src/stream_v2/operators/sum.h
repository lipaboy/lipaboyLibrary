#pragma once

#include "tools.h"

namespace lipaboy_lib {

	namespace stream_v2_space {

		namespace operators_space {

			struct sum {
				static constexpr OperatorMetaTypeEnum metaInfo = SUM;
				static constexpr bool isTerminated = true;

				template <class T>
				using RetType = T;

				template <class TStream>
				auto apply(TStream & stream) -> typename TStream::ResultValueType
				{
					using TResult = typename TStream::ResultValueType;
					auto result = (stream.hasNext()) ? stream.nextElem() : TResult();
					for (; stream.hasNext();)
						result += stream.nextElem();
					return std::move(result);
				}
			};

		}

	}

}