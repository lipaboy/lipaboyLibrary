#pragma once

#include "tools.h"

namespace lipaboy_lib::stream_space {

	namespace operators {

		struct skip : TReturnSameType
		{
		public:
			using size_type = size_t;

		public:
			skip(size_type count) : count_(count) {}

			template <class TSubStream>
			auto nextElem(TSubStream& stream) -> typename TSubStream::ResultValueType {
				skipElements<TSubStream>(stream);
				return stream.nextElem();
			}

			template <class TSubStream>
			void incrementSlider(TSubStream& stream) { 
				skipElements<TSubStream>(stream);
				stream.incrementSlider(); 
			}

			template <class TSubStream>
			bool hasNext(TSubStream& stream) {
				skipElements<TSubStream>(stream);
				return stream.hasNext(); 
			}

			size_type count() const { return count_; }

		private:
			template <class TSubStream>
			void skipElements(TSubStream& stream) {
				if (!isSkipped) {
					for (size_type i = 0; i < count() && stream.hasNext(); i++)
						stream.incrementSlider();
					isSkipped = true;
				}
			}

		private:
			size_type count_;
			bool isSkipped = false;
		};

	}

}

