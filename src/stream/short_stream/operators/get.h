#pragma once

#include "tools.h"

namespace lipaboy_lib::short_stream {

	namespace operators {

		struct get : public TReturnSameType
		{
		public:
			using size_type = size_t;

			static constexpr bool isTerminated = false;
		public:
			get(size_type size) : size_(size) {}

			template <class TSubStream>
			auto next(TSubStream& stream) -> RetType<typename TSubStream::ResultValueType> {
				auto resOpt = stream.next();
				if (size_ > 0u && resOpt != std::nullopt) {
					--size_;
					return resOpt;
				}
				return std::nullopt;
			}

			template <class TSubStream>
			void incrementSlider(TSubStream& stream) {
				size_ = (size_ > 0u) ? size_ - 1u : size_;
				stream.incrementSlider();
			}

			size_type size() const { return size_; }

		private:
			size_type size_;
		};

	}

}

