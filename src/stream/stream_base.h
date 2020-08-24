#pragma once

#include "stream_extended.h"
#include "extra_tools/producing_iterator.h"
#include "extra_tools/initializer_list_iterator.h"

#include <vector>
#include <functional>
#include <algorithm>
#include <iterator>
#include <type_traits>
#include <optional>
#include <typeinfo>

#include <iostream>

namespace lipaboy_lib {

	namespace stream_space {

		using std::vector;
		using std::pair;
		using std::string;
		using std::optional;

		using std::cout;

		using lipaboy_lib::ProducingIterator;
		using lipaboy_lib::InitializerListIterator;


		//--------------------------Stream Base (specialization class)----------------------//

		template <class TIterator>
		class StreamBase<TIterator> {
		public:
			using T = typename std::iterator_traits<TIterator>::value_type;
			using ValueType = T;
			using size_type = size_t;
			using outside_iterator = TIterator;
			using GeneratorTypePtr = std::function<ValueType(void)>;

		public:
			template <class Functor>
			using ExtendedStreamType = StreamBase<Functor, TIterator>;

			using ResultValueType = ValueType;

		public:
			// INFO: this friendship means that all the Streams, which is extended from current,
			//		 can have access to current class method API.
			template <typename, typename...> friend class StreamBase;

		public:
			//----------------------Constructors----------------------//

			template <class OuterIterator>
			explicit
				StreamBase(OuterIterator begin, OuterIterator end)
				: begin_(begin),
				end_(end)
			{}
			explicit
				StreamBase(std::initializer_list<T> init)
				: begin_(init)
			{
				if constexpr (StreamBase::isInitializingListCreation())
					end_ = begin_.endIter();
			}
			explicit
				StreamBase(GeneratorTypePtr generator)
				: begin_(generator),
				end_()
			{}

			//----------------------Methods API-----------------------//

		protected:
			static constexpr bool isNoGetTypeBefore() { return true; }
			static constexpr bool isGeneratorProducing() {
				return std::is_same_v<TIterator, ProducingIterator<ValueType> >;
			}
			static constexpr bool isInitializingListCreation() {
				return std::is_same_v<TIterator, InitializerListIterator<ValueType>
					//typename std::initializer_list<ValueType>::iterator
				>;
			}
			static constexpr bool isOutsideIteratorsRefer() {
				return !isGeneratorProducing() && !isInitializingListCreation();
			}

		public:
			inline static constexpr bool isInfinite() {
				return isGeneratorProducing() && isNoGetTypeBefore();
			}
			template <class TStream_>
			inline static constexpr void assertOnInfinite() {
				static_assert(!TStream_::isInfinite(),
					"Stream error: attempt to work with infinite stream");
			}

		protected:
			// Info:
			// illusion of protected (it means that can be replace on private)
			// (because all the variadic templates are friends
			// from current Stream to first specialization) (it is not a real inheritance)


			//-----------------Slider API--------------//
		public:
			ResultValueType nextElem() {
				auto elem = //std::forward<T>(
					*begin_;
					//);
				begin_++;
				return elem;
			}
			bool hasNext() { return begin_ != end_; }
			void incrementSlider() { begin_++; }

			//-----------------Slider API Ends--------------//

		public:
			bool operator==(StreamBase const & other) const { return equals(other); }
			bool operator!=(StreamBase const & other) const { return !((*this) == other); }
		private:
			bool equals(StreamBase const & other) const {
				return begin_ == other.begin_
					&& end_ == other.end_;
			}

		private:
			TIterator begin_;
			TIterator end_;
		};

	}

}
