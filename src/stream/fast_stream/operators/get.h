#pragma once

#include "tools.h"

namespace lipaboy_lib::fast_stream {

	namespace operators {

		struct get :
			public lipaboy_lib::stream_space::op::get
		{
            static constexpr bool isTerminated = false;
		public:
			get(size_type size) : lipaboy_lib::stream_space::op::get(size) {}

			template <class TSubStream>
			void initialize(TSubStream& stream) {
				stream.initialize();
			}
		};

	}

}

