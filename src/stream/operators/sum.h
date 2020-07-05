#pragma once

#include <any>
#include "tools.h"

namespace lipaboy_lib {

	namespace stream_space {

		namespace operators {

			// Question: what's shit is it doing here, void* ?

			template <class TInit = void*>
			struct sum : TReturnSameType {
				static constexpr OperatorMetaTypeEnum metaInfo = SUM;
				static constexpr bool isTerminated = true;

				sum() {
				}

				sum(TInit init) {
					init_ = init;
				}

				template <class TStream>
				auto apply(TStream & stream) -> typename TStream::ResultValueType
				{
					using TResult = typename TStream::ResultValueType;
					TResult result;
					if constexpr (std::is_same_v<TInit, void *>)
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

			using stream_space::operators::OperatorMetaTypeEnum;
			using SuperType = stream_space::operators::template sum<>;

			struct sum : public SuperType
			{
				static constexpr bool isTerminated = true;

				template <class TStream>
				auto apply(TStream & stream) -> typename TStream::ResultValueType
				{
					stream.initialize();
					return static_cast<SuperType *>(this)->apply(stream);
				}
			};

		}

	}

}