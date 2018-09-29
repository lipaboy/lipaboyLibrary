#pragma once

#include "tools.h"

namespace lipaboy_lib {

	namespace stream {

		namespace operators {

			struct sum : TReturnSameType {
				static constexpr OperatorMetaTypeEnum metaInfo = SUM;
				static constexpr bool isTerminated = true;

				template <class TStream>
				auto apply(TStream & stream) -> typename TStream::ResultValueType
				{
					using TResult = typename TStream::ResultValueType;
					auto result = (stream.hasNext()) ? std::move(stream.nextElem()) : TResult();
					while (stream.hasNext())
						result += std::move(stream.nextElem());
					return std::move(result);
				}
			};

		}

	}

	namespace fast_stream {

		namespace operators {

			using stream::operators::OperatorMetaTypeEnum;

			struct sum : 
				public stream::operators::sum
			{
				static constexpr bool isTerminated = true;

				template <class TStream>
				auto apply(TStream & stream) -> typename TStream::ResultValueType
				{
					stream.initialize();
					return static_cast<stream::operators::sum *>(this)->apply(stream);
				}
			};

		}

	}

}