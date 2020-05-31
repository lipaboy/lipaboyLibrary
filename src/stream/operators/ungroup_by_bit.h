#pragma once

#include "tools.h"

#include <vector>
#include <type_traits>
#include <memory>

namespace lipaboy_lib {

	namespace stream_space {

		// TODO: refactor it
		namespace operators {

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

				// TODO: move allocating memory to Slider Api
				ungroup_by_bit_impl(ungroup_by_bit const &)
				{}

				template <class TSubStream>
				RetType<T> nextElem(TSubStream& stream) {
					init(stream);
					auto res = currentElem(stream);
					incrementSlider(stream);
					return res;
				}

				template <class TSubStream>
				void incrementSlider(TSubStream& stream) {
					using TResult = typename TSubStream::ResultValueType;
					constexpr size_type BITS_COUNT_OF_TYPE = 8 * sizeof(TResult);

					init(stream);
					++currBit_;
					if (currBit_ == BITS_COUNT_OF_TYPE) {
						// strange body of condition
						if (stream.hasNext())
							*pCurrentElem_ = std::move(stream.nextElem());
						else
							pCurrentElem_ = nullptr;
						currBit_ = 0;
					}
				}

				template <class TSubStream>
				bool hasNext(TSubStream& stream) {
					return pCurrentElem_ != nullptr || stream.hasNext(); 
				}

			private:
				template <class TSubStream>
				RetType<T> currentElem(TSubStream& stream) {
					return ((*pCurrentElem_) & (1 << currBit_)) >> currBit_;
				}

				template <class TSubStream>
				void init(TSubStream& stream) {
					if (pCurrentElem_ == nullptr && stream.hasNext()) {
						pCurrentElem_ = std::make_shared<CurrentValueType>(std::move(stream.nextElem()));
						currBit_ = 0;
					}
				}

			private:
				size_type currBit_ = 0;
				CurrentValueTypePtr pCurrentElem_ = nullptr;
			};

		}


		using operators::ungroup_by_bit;
		using operators::ungroup_by_bit_impl;

		template <class TStream>
		struct shortening::StreamTypeExtender<TStream, ungroup_by_bit> {
			template <class T>
			using remref = std::remove_reference_t<T>;

			using type = typename remref<TStream>::template ExtendedStreamType<
				remref<ungroup_by_bit_impl<typename TStream::ResultValueType> > >;
		};
	}

}
