#pragma once

#include "tools.h"

namespace lipaboy_lib {

	namespace stream_space {

		namespace operators_space {

			template <class Transform>
			struct map : public FunctorHolder<Transform> {
			public:
				template <class T>
				using RetType = typename std::result_of<Transform(T)>::type;

				static constexpr OperatorMetaTypeEnum metaInfo = MAP;
				static constexpr bool isTerminated = false;
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

}