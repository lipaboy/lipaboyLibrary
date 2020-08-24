#pragma once

#include "tools.h"

namespace lipaboy_lib::stream_space {

	namespace operators {

		template <class TypeCastTo>
		class cast_to {
		public:
			static constexpr bool isTerminated = false;

			template <class T>
			using RetType = TypeCastTo; // return the same type

		public:
			template <class StreamType>
			auto nextElem(StreamType& stream)
				-> RetType<typename StreamType::ResultValueType>
			{
				return TypeCastTo(stream.nextElem());
			}

			template <class StreamType>
			void incrementElem(StreamType& stream)
			{
				stream.incrementElem();
			}

			template <class StreamType>
			bool hasNext(StreamType& stream)
			{
				return stream.hasNext();
			}
		};

		template <class TypeCastTo>
		class cast_static {
		public:
			static constexpr bool isTerminated = false;

			template <class T>
			using RetType = TypeCastTo; // return the same type

		public:
			template <class StreamType>
			auto nextElem(StreamType& stream)
				-> RetType<typename StreamType::ResultValueType>
			{
				return static_cast<TypeCastTo>(stream.nextElem());
			}

			template <class StreamType>
			void incrementElem(StreamType& stream)
			{
				stream.incrementElem();
			}

			template <class StreamType>
			bool hasNext(StreamType& stream)
			{
				return stream.hasNext();
			}
		};

	}

}