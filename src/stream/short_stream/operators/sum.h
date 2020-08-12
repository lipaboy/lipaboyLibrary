#pragma once

#include "tools.h"

namespace lipaboy_lib::short_stream {

	namespace operators {

		struct sum : TReturnSameType {
			static constexpr bool isTerminated = true;

			template <class TStream>
			auto apply(TStream& stream) -> RetType<typename TStream::ResultValueType>
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

