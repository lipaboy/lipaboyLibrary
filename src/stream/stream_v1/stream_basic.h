#pragma once

#include "stream_extended.h"
#include "extra_tools/extra_tools.h"
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

namespace stream_v1_space {

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

    template <typename, typename...> friend class Stream;

	//using SubType = void;
	//using OperatorType = TIterator;

public:
    //----------------------Constructors----------------------//

    template <class OuterIterator>
    explicit
		Stream(OuterIterator begin, OuterIterator end) : range_(begin, end) {}
    explicit
		Stream(std::initializer_list<T> init) : range_(init) {}
	//template <size_type size>
	//explicit
	//	Stream(T(&init)[size]) : range_(init) {}

    Stream(Stream const & obj) : range_(obj.range_)
    {
#ifdef LOL_DEBUG_NOISY
        cout << "   StreamBase copy-constructed" << endl;
#endif
    }
    Stream(Stream&& obj) noexcept : range_(std::move(obj.range_))
    {
#ifdef LOL_DEBUG_NOISY
        cout << "   StreamBase move-constructed" << endl;
#endif
    }
    explicit
    Stream(typename Range::GeneratorTypePtr generator) : range_(generator) {}

    //----------------------Methods API-----------------------//

protected:
    Range & range() { return range_; }
    const Range & range() const { return range_; }
protected:
    static constexpr bool isNoGetTypeBefore() { return true; }
    static constexpr bool isGeneratorProducing() {
		return std::is_same_v<TIterator, ProducingIterator<ValueType> >;
	}
	static constexpr bool isInitilizerListCreation() {
		return std::is_same_v<TIterator, InitializerListIterator<ValueType> 
			//typename std::initializer_list<ValueType>::iterator
		>;
	}
	static constexpr bool isOutsideIteratorsRefer() {
		return !isGeneratorProducing() && !isInitilizerListCreation();
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
	void init() { range().init(); }
    ResultValueType nextElem() { return std::move(range().nextElem()); }
    ValueType currentElem() { return std::move(range().currentElem()); }
    bool hasNext() { return range().hasNext(); }
	void incrementSlider() { range().incrementSlider(); }

    //-----------------Slider API Ends--------------//

public:
    bool operator==(Stream const & other) const { return equals(other); }
    bool operator!=(Stream const & other) const { return !((*this) == other); }
private:
    bool equals(Stream const & other) const { return range_.equals(other.range_); }

private:
    Range range_;
};

}

}
