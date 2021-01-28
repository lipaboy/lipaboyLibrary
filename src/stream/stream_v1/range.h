#pragma once

#include "stream_basic.h"
#include "extra_tools/extra_tools.h"

#include <memory>
#include <vector>
#include <functional>
#include <algorithm>
#include <iterator>
#include <type_traits>

namespace lipaboy_lib {

namespace stream_v1_space {

using std::vector;
using std::pair;
using std::string;
using std::unique_ptr;
using std::shared_ptr;

//--------------------RangeType-----------------------//

template <class TIterator>
class Stream<TIterator>::Range {
public:
    using ValueType = typename Stream::ValueType;
    using reference = ValueType&;
    using const_reference = const reference;
    using size_type = typename Stream::size_type;
    using TIndex = size_type;
    // TODO: remove this crutch (Visual Studio compiler cannot resolve it)
	using GeneratorTypePtr = //typename Stream::GeneneratorTypePtr;
		std::function<ValueType(void)>;

public:
    explicit
		Range(std::initializer_list<ValueType> init)
        : firstIter_(init)
    {
		if constexpr (Stream::isInitilizerListCreation())
				lastIter_ = firstIter_.endIter();
	}
    template <class OuterIterator>
    explicit
		Range(OuterIterator begin, OuterIterator end)
        : firstIter_(begin), 
		lastIter_(end)
    {}
    explicit
		Range(GeneratorTypePtr generator)
		: firstIter_(generator), 
		lastIter_()
    {}

protected:
    TIterator firstIter() const { return firstIter_; }
    TIterator lastIter() const { return lastIter_; }

public:

    //-----------------Slider API---------------//

	void init() {}
	ValueType nextElem() {
		ValueType value = *(firstIter());
		// Note: you can't optimize it because for istreambuf_iterator
		//       post-increment operator has unspecified by standard
		// Note(2): upper note has the same effect with ProducingIterator<T>
		//			maybe this effect the same for all the input-iterators
		++firstIter_; // maybe replace it to std::next
		return std::move(value);
	}
	void incrementSlider() {
		// Note: you can't optimize it because for istreambuf_iterator
		//       post-increment operator has unspecified by standard
		++firstIter_;
	}
    ValueType currentElem() const { return *firstIter(); }
    // Linux (gcc): why I can't replace it on getter samples
    bool hasNext() { return firstIter_ != lastIter_; }

public:

    bool equals(Range const & other) const {
		// TODO: maybe add comparison for size_
                return (firstIter() == other.firstIter()
                        && lastIter() == other.lastIter());
    }

private:
    // Iterators that refer to outside of class container
    TIterator firstIter_;
    TIterator lastIter_;

};

}

}

