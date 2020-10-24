#pragma once

#include "tools.h"
#include "extra_tools/extra_tools.h"

namespace lipaboy_lib::stream_space {

	namespace operators {

		template <class Transform>
		struct map : public FunctorHolder<Transform> {
		public:
			template <class T>
			using RetType = std::invoke_result_t <Transform, T>;
		public:
			map(Transform functor) : FunctorHolder<Transform>(functor) {}

			template <class TSubStream>
			auto nextElem(TSubStream& stream) 
				-> RetType<typename TSubStream::ResultValueType> 
			{
				return std::move(FunctorHolder<Transform>::functor()(stream.nextElem()));
			}

			template <class TSubStream>
			void incrementSlider(TSubStream& stream) { stream.incrementSlider(); }

			template <class TSubStream>
			bool hasNext(TSubStream& stream) { return stream.hasNext(); }
		};

	}

}
