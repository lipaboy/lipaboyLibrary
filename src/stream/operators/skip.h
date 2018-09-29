#pragma once

#include "tools.h"

namespace lipaboy_lib {

	namespace stream {

		namespace operators {

			struct skip : TReturnSameType 
			{
			public:
				using size_type = size_t;

				static constexpr OperatorMetaTypeEnum metaInfo = SKIP;
				static constexpr bool isTerminated = false;
			public:
				skip(size_type count) : count_(count) {}

				template <class TSubStream>
				auto nextElem(TSubStream& stream) -> typename TSubStream::ResultValueType {
					skipElements<TSubStream>(stream);
					return std::move(stream.nextElem());
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

	namespace fast_stream {

		namespace operators {

			struct skip : 
				public stream::operators::TReturnSameType
			{
			public:
				using size_type = size_t;

				static constexpr bool isTerminated = false;
			public:
				skip(size_type count) : count_(count) {}

				template <class TSubStream>
				auto nextElem(TSubStream& stream) -> typename TSubStream::ResultValueType {
					return std::move(stream.nextElem());
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

}