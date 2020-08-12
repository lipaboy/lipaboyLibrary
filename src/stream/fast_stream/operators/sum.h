#pragma once

#include <any>
#include "tools.h"

namespace lipaboy_lib::fast_stream {

	namespace operators {

		using SuperType = stream_space::operators::template sum<>;

		struct sum : public SuperType
		{
			static constexpr bool isTerminated = true;

			template <class TStream>
			auto apply(TStream& stream) -> typename TStream::ResultValueType
			{
				stream.initialize();
				return static_cast<SuperType*>(this)->apply(stream);
			}
		};

	}

}

