#pragma once

#include "tools.h"

#include <vector>

namespace lipaboy_lib {

	namespace stream_space {

		namespace operators_space {

			using std::vector;

			struct to_vector {
			public:
				template <class T>
				using RetType = std::vector<T>;

				static constexpr OperatorMetaTypeEnum metaInfo = TO_VECTOR;
				static constexpr bool isTerminated = true;
			public:

				template <class Stream_>
				auto apply(Stream_ & obj) -> vector<typename Stream_::ResultValueType>
				{
					using ToVectorType = vector<typename Stream_::ResultValueType>;
					ToVectorType toVector;
					for (; obj.hasNext(); )
						toVector.push_back(obj.nextElem());
					return std::move(toVector);
				}
			};

		}

	}

}