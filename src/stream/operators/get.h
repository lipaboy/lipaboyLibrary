#pragma once

#include "tools.h"

namespace lipaboy_lib {

	namespace stream {

		namespace operators {

			struct get : public TReturnSameType
			{
			public:
				using size_type = size_t;

				static constexpr OperatorMetaTypeEnum metaInfo = GET;
				static constexpr bool isTerminated = false;
			public:
				get(size_type size) : size_(size) {}

				template <class TSubStream>
				auto nextElem(TSubStream& stream) -> typename TSubStream::ResultValueType {
					--size_;
					return stream.nextElem();
				}

				template <class TSubStream>
				void incrementSlider(TSubStream& stream) {
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

	namespace fast_stream {

		namespace operators {

			struct get : 
				public stream::operators::get
			{
			public:
				get(size_type size) : stream::operators::get(size) {}

				template <class TSubStream>
				void initialize(TSubStream& stream) {
					stream.initialize();
				}
			};

		}

	}

}