#pragma once

#include <any>
#include "tools.h"

namespace lipaboy_lib {

	namespace stream {

		namespace operators {

			// TODO
			// 1) Replace std::any to template sum<>

			struct sum : TReturnSameType {
				static constexpr OperatorMetaTypeEnum metaInfo = SUM;
				static constexpr bool isTerminated = true;

				sum() {
				}

				template <class TInit>
				sum(TInit init) {
					init_ = init;
				}

				template <class TStream>
				auto apply(TStream & stream) -> typename TStream::ResultValueType
				{
					using TResult = typename TStream::ResultValueType;
					auto result = init_.has_value() ? std::any_cast<TResult>(init_) : TResult();
						//(stream.hasNext()) ? stream.nextElem() : TResult();
					while (stream.hasNext())
						result += stream.nextElem();
					return result;
				}

				std::any init_;
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