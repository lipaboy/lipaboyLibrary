#pragma once

#include "tools.h"
#include "extra_tools/extra_tools.h"

namespace lipaboy_lib::stream_space {

	namespace op {

		template <class Transform>
		struct map : public impl::FunctorHolder<Transform>
		{
		public:
			using FHolder = impl::FunctorHolder<Transform>;

			template <class T>
			using RetType = std::invoke_result_t<Transform, T>;
		public:
			map(Transform functor) : FHolder(functor) {}

			template <class TSubStream>
			auto nextElem(TSubStream& stream) 
				-> RetType<typename TSubStream::ResultValueType> 
			{
				return std::move(FHolder::functor()(stream.nextElem()));
			}

			template <class TSubStream>
			void incrementSlider(TSubStream& stream) { stream.incrementSlider(); }

			template <class TSubStream>
			bool hasNext(TSubStream& stream) { return stream.hasNext(); }
		};

	}

}
