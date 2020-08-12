#pragma once

#include "tools.h"

namespace lipaboy_lib::fast_stream {

	namespace operators {

		struct skip :
			public stream_space::operators::TReturnSameType
		{
		public:
			using size_type = size_t;

			static constexpr bool isTerminated = false;
		public:
			skip(size_type count) : count_(count) {}

			template <class TSubStream>
			auto nextElem(TSubStream& stream) -> typename TSubStream::ResultValueType {
				return stream.nextElem();
			}

			template <class TSubStream>
			void incrementSlider(TSubStream& stream) {
				stream.incrementSlider();
			}

			template <class TSubStream>
			bool hasNext(TSubStream& stream) {
				return stream.hasNext();
			}

			template <class TSubStream>
			void initialize(TSubStream& stream) {
				stream.initialize();
				for (; count_ > 0 && stream.hasNext(); --count_) {
					stream.incrementSlider();
				}
			}

		private:
			size_type count_;
		};

	}

}

