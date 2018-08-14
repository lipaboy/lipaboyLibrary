#pragma once

#include "tools.h"

#include <vector>
#include <type_traits>
#include <memory>

namespace lipaboy_lib {

namespace stream_space {

	// TODO: refactor it
	namespace operators_space {

		using std::vector;
		using std::shared_ptr;

		using std::cout;
		using std::endl;

		struct ungroup_by_bit {
		public:
			template <class Arg>
			using RetType = bool;

			//static constexpr OperatorMetaTypeEnum metaInfo = UNGROUP_BY_BIT;
			static constexpr bool isTerminated = false;
		};

		template <class T>
		struct ungroup_by_bit_impl {
		public:
			using size_type = size_t;

			template <class Arg>
			using RetType = bool;

			using CurrentValueType = T;
			using CurrentValueTypePtr = shared_ptr<T>;

			static constexpr OperatorMetaTypeEnum metaInfo = UNGROUP_BY_BIT;
			static constexpr bool isTerminated = false;
		public:
			ungroup_by_bit_impl(ungroup_by_bit const &) 
			{}

			template <class TSubStream>
			void init(TSubStream& stream)
			{
				currBit_ = 0;
				pCurrentElem_ = std::make_shared<CurrentValueType>(stream.currentElem());
			}

			template <class TSubStream>
			auto nextElem(TSubStream& stream) -> typename TSubStream::ResultValueType {
				auto res = currentElem<TSubStream>(stream);
				incrementSlider<TSubStream>(stream);
				return res;
			}

			template <class TSubStream>
			void incrementSlider(TSubStream& stream) {
				using TResult = typename TSubStream::ResultValueType;
				constexpr size_type BITS_COUNT_OF_TYPE = 8 * sizeof(TResult);

				currBit_ = currBit_ + 1;	//iter++;
				if (currBit_ == BITS_COUNT_OF_TYPE) {
					// strange body of condition
					stream.incrementSlider();
					if (stream.hasNext())
						*pCurrentElem_ = stream.currentElem();
					else
						pCurrentElem_ = nullptr;
					currBit_ = 0;
				}
			}

			template <class TSubStream>
			bool currentElem(TSubStream& stream) {
				return ((*pCurrentElem_) & (1 << currBit_)) >> currBit_;
			}

			template <class TSubStream>
			bool hasNext(TSubStream& stream) { return currBit_ != 0 || stream.hasNext(); }

		private:
			size_type currBit_;
			CurrentValueTypePtr pCurrentElem_;
		};

	}


	using operators_space::ungroup_by_bit;
	using operators_space::ungroup_by_bit_impl;

	template <class TStream>
	struct shortening::StreamTypeExtender<TStream, ungroup_by_bit> {
		template <class T>
		using remref = std::remove_reference_t<T>;

		using type = typename remref<TStream>::template ExtendedStreamType<
			remref<ungroup_by_bit_impl<typename TStream::ResultValueType> > >;
	};
}

}
