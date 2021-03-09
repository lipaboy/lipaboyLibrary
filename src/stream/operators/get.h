#pragma once

#include "tools.h"

namespace lipaboy_lib::stream_space {

	namespace operators {

		struct get : 
			impl::TReturnSameType, 
			impl::FixSizeOperator
		{
		public:
			using size_type = size_t;

		public:
			get(size_type size) : size_(size) {}

			template <class TSubStream>
			auto nextElem(TSubStream& stream) -> typename TSubStream::ResultValueType {
				// INFO: you needn't to check if there are not elements because
				//		it must doing the client by calling hasNext()
				//size_ = (size_ > 0) ? size_ - 1 : size_;
				--size_;
				return stream.nextElem();
			}

			template <class TSubStream>
			void incrementSlider(TSubStream& stream) {
				//size_ = (size_ > 0) ? size_ - 1 : size_;
				--size_;
				stream.incrementSlider();
			}

			template <class TSubStream>
			bool hasNext(TSubStream& stream) { return size() > 0 && stream.hasNext(); }

			size_type size() const { return size_; }

			bool operator==(get const & other) const { return size() == other.size(); }
			bool operator!=(get const & other) const { return !(*this == other); }

		private:
			size_type size_;
		};

	}

}
