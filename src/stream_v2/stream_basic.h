#pragma once

#include "stream_extended.h"
#include "extra_tools/producing_iterator.h"
#include "extra_tools/initializer_list_iterator.h"

#include <vector>
#include <functional>
#include <algorithm>
#include <iterator>
#include <type_traits>

#include <typeinfo>

#include <iostream>

namespace lipaboy_lib {

	namespace stream_v2_space {

		using std::vector;
		using std::pair;
		using std::string;

		using std::cout;


		//--------------------------Stream Base (specialization class)----------------------//

                template <class TIterator>
		class Stream<TIterator> {
		public:
			using T = typename std::iterator_traits<TIterator>::value_type;
			using ValueType = T;
			using size_type = size_t;
			using outside_iterator = TIterator;
			class Range;

			template <class Functor>
			using ExtendedStreamType = Stream<Functor, TIterator>;

			using ResultValueType = ValueType;

			using GeneratorTypePtr = std::function<ValueType(void)>;

			template <typename, typename...> friend class Stream;

			//using SuperType = void;
			//using OperatorType = TIterator;

		public:
			//----------------------Constructors----------------------//

			template <class OuterIterator>
			explicit
				Stream(OuterIterator begin, OuterIterator end) 
				: begin_(begin),
				end_(end)
			{}
			explicit
				Stream(std::initializer_list<T> init) 
				: begin_(init)
			{
				if constexpr (Stream::isInitializingListCreation())
						end_ = begin_.endIter();
			}
			explicit
                                Stream(GeneratorTypePtr generator)
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
				auto elem = *begin_;
				begin_++;
				return std::move(elem); 
			}
			bool hasNext() { return begin_ != end_; }
			void incrementSlider() { begin_++; }

			//-----------------Slider API Ends--------------//

		public:
			bool operator==(Stream const & other) const { return equals(other); }
			bool operator!=(Stream const & other) const { return !((*this) == other); }
		private:
			bool equals(Stream const & other) const { 
				return begin_ == other.begin_ 
					&& end_ == other.end_; 
			}

		private:
			TIterator begin_;
			TIterator end_;
		};

	}

}
