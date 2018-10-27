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
					auto result = (stream.hasNext()) ? stream.nextElem() : TResult();
					while (stream.hasNext())
						result += stream.nextElem();
					return result;
				}
			};

		}

	}

	namespace short_stream {

		namespace operators {

			struct sum : TReturnSameType {
				static constexpr bool isTerminated = true;

				template <class TStream>
				auto apply(TStream & stream) -> RetType<typename TStream::ResultValueType>
				{
					using TResult = typename TStream::ResultValueType::value_type;
					auto resultOpt = stream.next();

					if (resultOpt == std::nullopt)
						return std::nullopt;

					TResult result = std::move(resultOpt.value());
					while (true) {
						resultOpt = stream.next();
						if (resultOpt == std::nullopt)
							break;
						result += std::move(resultOpt.value());
					}

					return result;
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