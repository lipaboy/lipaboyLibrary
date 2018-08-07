#pragma once

#include "stream_base.h"

#include <memory>
#include <vector>
#include <functional>
#include <algorithm>
#include <iterator>
#include <type_traits>

#include <iostream>

namespace stream_space {

using std::vector;
using std::pair;
using std::string;
using std::unique_ptr;
using std::shared_ptr;

// For debugging
using std::cout;
using std::endl;

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
    Range(std::initializer_list<T> init)
        : firstIter_(init), 
		lastIter_(), 
		size_(init.size())
    {}
    template <class OuterIterator>
    Range(OuterIterator begin, OuterIterator end)
        : firstIter_(begin), 
		lastIter_(end),
		isSizeSet_(false)
    {}
	Range(GeneratorTypePtr generator)
		: firstIter_(generator), 
		lastIter_(), 
		isInfinite_(true)
    {}

	// TODO : make constructors the default
    Range(const Range& obj)
        : firstIter_(obj.firstIter_),
		lastIter_(obj.lastIter_),
        isInfinite_(obj.isInfinite_), 
		isSizeSet_(obj.isSizeSet_), 
		size_(obj.size_)
    {
#ifdef LOL_DEBUG_NOISY
        cout << " Range copy-constructed" << endl;
#endif
    }
    Range(Range&& obj) noexcept
        : firstIter_(std::move(obj.firstIter_)), 
		lastIter_(std::move(obj.lastIter_)),
        isInfinite_(obj.isInfinite_), 
		isSizeSet_(obj.isSizeSet_), 
		size_(obj.size_)
    {
#ifdef LOL_DEBUG_NOISY
        cout << " Range move-constructed" << endl;
#endif
    }

protected:
    TIterator outsideBegin() const { return firstIter_; }
    TIterator outsideEnd() const { return lastIter_; }

public:
    ValueType get(size_type ind) const {
        auto iter = outsideBegin();
        std::advance(iter, ind);
        return *iter;
    }

private:
    void setSize(size_type newSize) {
        if constexpr (isOutsideIteratorsRefer()) {
                size_ = newSize;
                isSizeSet_ = true;
        }
        else if constexpr (isGeneratorProducing()) 
                size_ = newSize;
        else // InitializerList
				size_ = (size_ >= newSize) ? newSize : size_;
    }

public:
    bool isInfinite() const { return isInfinite_; }
	// INFO: you cannot move it into setSize because if you do so
	//		 then Skip operation can make Infinite stream not infinite
	//		 (see moveIterBegin() method)
	void makeFinite(size_type newSize) {
		setSize(newSize);
		isInfinite_ = false;
	}

    void doPreliminaryActions() {}

    //-----------------Slider API---------------//

    void init() {
		// !! Problem with double initializing tempValue (when initSlider is called twice -> nth(0); nth(0); )
    }
    ValueType nextElem() {
        if constexpr (isGeneratorProducing()) {
                size_ = (hasNext()) ? size_ - 1 : size_;
				auto currElem = std::move(*outsideBegin());
				++firstIter_;
                return std::move(currElem);
        }
		else if constexpr (isInitilizerListCreation()) {
				size_ = (hasNext()) ? size_ - 1 : size_;
				return std::move(*(firstIter_++));
		}
        else // constexpr isOutsideIteratorsRefer()
		{
                ValueType value = *(outsideBegin());
                // Note: you can't optimize it because for istreambuf_iterator
                //       post-increment operator has unspecified by standard
                ++firstIter_; // maybe replace it to std::next
                size_ = (hasNext()) ? size_ - 1 : size_;
                return std::move(value);
        }
    }
    void incrementSlider() {
        if constexpr (isGeneratorProducing()) {
				++firstIter_;
                size_ = (hasNext()) ? size_ - 1 : size_;
        }
		else if constexpr (isInitilizerListCreation()) {
				++firstIter_;
				size_ = (hasNext()) ? size_ - 1 : size_;
		}
        else // constexpr isOutsideIteratorsRefer()
		{
                // Note: you can't optimize it because for istreambuf_iterator
                //       post-increment operator has unspecified by standard
                ++firstIter_;
				size_ = (hasNext()) ? size_ - 1 : size_;
        }
    }
    ValueType currentElem() const {
            return *outsideBegin();
    }
    bool hasNext() const {
        if constexpr (isGeneratorProducing())
                return (size_ > 0);
        else if constexpr (isInitilizerListCreation())
                return (size_ > 0);
        else    // until
                return !(outsideBegin() == outsideEnd() || (isSizeSet_ && size_ <= 0));
    }

public:

    bool equals(Range & other) {
		// TODO: maybe add comparison for size_
                return (outsideBegin() == other.outsideBegin()
                        && outsideEnd() == other.outsideEnd());
    }

private:
    // Iterators that refer to outside of class container
    TIterator firstIter_;
    TIterator lastIter_;

	// If you want to exclude useless fields (e.g. lastIter_ from InitializerListCreation or GeneratorProducing)
	// then you can pass necessary fields through wrapper: IteratorPair (begin-end, begin-size, begin-size-isInfinite)
	bool isInfinite_ = false;
    bool isSizeSet_ = false;
    size_type size_;

public:
    void moveBeginIter(size_type position) {
		std::advance(firstIter_, position);
		if constexpr (isGeneratorProducing() || isInitilizerListCreation()) 
				setSize(size_ - position);
		else // constexpr
				if (isSizeSet_)
					setSize(size_ - position);
    }


};

}
