#pragma once

#include "tools.h"

#include <vector>

namespace lipaboy_lib::stream_space {

	namespace operators {

		using std::vector;

		struct to_vector : TerminatedOperator
		{
		public:
			template <class T>
			using RetType = std::vector<T>;
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
