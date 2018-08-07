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

template <class TRange>
struct GetRangeIter<true, TRange> {
    using TIterator = typename TRange::OwnIterator;
    static TIterator begin(const TRange& obj) { return obj.ownBegin(); }
    static TIterator end(const TRange& obj) { return obj.ownEnd(); }
};
template <class TRange>
struct GetRangeIter<false, TRange> {
    using TIterator = typename TRange::OutsideIterator;
    static TIterator begin(const TRange& obj) { return obj.outsideBegin(); }
    static TIterator end(const TRange& obj) { return obj.outsideEnd(); }
};

template <class StorageInfo, class TIterator>
class Stream<StorageInfo, TIterator>::Range {
public:
    using ValueType = typename Stream::ValueType;
    using reference = ValueType&;
    using const_reference = const reference;
    using OutsideIterator = typename Stream::outside_iterator;
    using size_type = typename Stream::size_type;
    using TIndex = size_type;
    // TODO: remove this crutch (Visual Studio compiler cannot resolve it)
	using GeneratorTypePtr = //typename Stream::GeneneratorTypePtr;
		std::function<ValueType(void)>;
    using OwnContainerType = OwnContainerTypeWithoutValueType<ValueType>;
    using OwnContainerTypePtr = std::unique_ptr<OwnContainerType>;
    using OwnIterator = typename OwnContainerType::iterator;

public:
    Range(std::initializer_list<T> init)
        : outsideBegin_(init), 
		outsideEnd_(), 
		size_(init.size())
    {}
    template <class OuterIterator>
    Range(OuterIterator begin, OuterIterator end)
        : outsideBegin_(begin), 
		outsideEnd_(end),
		isSizeSet_(false)
    {}
	Range(GeneratorTypePtr generator)
		: outsideBegin_(generator), 
		outsideEnd_(), 
		isInfinite_(true)
    {}

	// TODO : make constructors the default
    Range(const Range& obj)
        : outsideBegin_(obj.outsideBegin_),
		outsideEnd_(obj.outsideEnd_),
        isInfinite_(obj.isInfinite_), 
		isSizeSet_(obj.isSizeSet_), 
		size_(obj.size_)
    {
#ifdef LOL_DEBUG_NOISY
        cout << " Range copy-constructed" << endl;
#endif
    }
    Range(Range&& obj) noexcept
        : outsideBegin_(std::move(obj.outsideBegin_)), 
		outsideEnd_(std::move(obj.outsideEnd_)),
        isInfinite_(obj.isInfinite_), 
		isSizeSet_(obj.isSizeSet_), 
		size_(obj.size_)
    {
#ifdef LOL_DEBUG_NOISY
        cout << " Range move-constructed" << endl;
#endif
    }

public:

    //------------Iterators API-------------//

    OutsideIterator outsideBegin() const { return outsideBegin_; }
    OutsideIterator outsideEnd() const { return outsideEnd_; }

public:
    ValueType get(size_type ind) const {
        auto iter = outsideBegin();
        std::advance(iter, ind);
        return *iter;
    }

    void setSize(size_type newSize) {
        if constexpr (StorageInfo::info == OUTSIDE_ITERATORS) {
                size_ = newSize;
                isSizeSet_ = true;
        }
        else if constexpr (StorageInfo::info == GENERATOR) {
                size_ = newSize;
        }
        else {
				size_ = (size_ >= newSize) ? newSize : size_;
			// move end-iter
                /*if (newSize <= ownEndIndex_ - ownBeginIndex_)
                    setOwnIndices(ownBeginIndex_, ownBeginIndex_ + newSize);*/
        }
    }

    //size_type size() const { return size_; }
    bool isInfinite() const { return isInfinite_; }
    void makeFinite(size_type size) {
        isInfinite_ = false;
        setSize(size);
    }

    void doPreliminaryActions() {}

    //-----------------Slider API---------------//

    void initSlider() {
		// !! Problem with double initializing tempValue (when initSlider is called twice -> nth(0); nth(0); )
    }
    ValueType nextElem() {
        if constexpr (StorageInfo::info == GENERATOR) {
                size_ = (hasNext()) ? size_ - 1 : size_;
				auto currElem = std::move(*outsideBegin());
				++outsideBegin_;
                return std::move(currElem);
        }
		else if constexpr (StorageInfo::info == INITIALIZING_LIST) {
				size_ = (hasNext()) ? size_ - 1 : size_;
				return std::move(*(outsideBegin_++));
		}
        else {
                ValueType value = *(outsideBegin());
                // Note: you can't optimize it because for istreambuf_iterator
                //       post-increment operator has unspecified by standard
                ++outsideBegin_; // maybe replace it to std::next
                size_ = (hasNext()) ? size_ - 1 : size_;
                return std::move(value);
        }
    }
    void incrementSlider() {
        if constexpr (StorageInfo::info == GENERATOR) {
				++outsideBegin_;
                size_ = (hasNext()) ? size_ - 1 : size_;
        }
		else if constexpr (StorageInfo::info == INITIALIZING_LIST) {
				++outsideBegin_;
				size_ = (hasNext()) ? size_ - 1 : size_;
		}
        else {
                // Note: you can't optimize it because for istreambuf_iterator
                //       post-increment operator has unspecified by standard
                ++outsideBegin_;
				size_ = (hasNext()) ? size_ - 1 : size_;
        }
    }
    ValueType currentElem() const {
        /*if constexpr (StorageInfo::info == GENERATOR)
                return *pTempValue_;
        else if constexpr (StorageInfo::info == INITIALIZING_LIST)
                return *ownIterSlider_;
        else*/
                return *outsideBegin();
    }
    bool hasNext() const {
        if constexpr (StorageInfo::info == GENERATOR)
                return (size_ > 0);
        else if constexpr (StorageInfo::info == INITIALIZING_LIST)
                return (size_ > 0);
        else    // until
                return !(outsideBegin() == outsideEnd() || (isSizeSet_ && size_ <= 0));
    }

protected:

public:

    bool equals(Range & other) {
        // INFO: it is the right condition because OwnContainer will be appeared when
        // terminated operation is applied to stream. Before that stream refers to
        // outer iterators (if it uses them from the beginning)
        //if constexpr (StorageInfo::info == OUTSIDE_ITERATORS)
                return (outsideBegin() == other.outsideBegin()
                        && outsideEnd() == other.outsideEnd()
                        );
        /*else if constexpr (StorageInfo::info == INITIALIZING_LIST)
                return true;
        else
                return true;*/
    }

private:
    // Iterators that refer to outside of class container
    OutsideIterator outsideBegin_;
    OutsideIterator outsideEnd_;

	// If you want to exclude useless fields (e.g. outsideEnd_ from InitializerListCreation or GeneratorProducing)
	// then you can pass necessary fields through wrapper: IteratorPair (begin-end, begin-size, begin-size-isInfinite)
	bool isInfinite_ = false;
    bool isSizeSet_ = false;
    size_type size_;

public:
    void moveBeginIter(size_type position) {
		if constexpr (isGeneratorProducing()) {
				std::advance(outsideBegin_, position);
				setSize(size_ - position);
		}
		else if constexpr (StorageInfo::info == INITIALIZING_LIST) {
				std::advance(outsideBegin_, position);
				setSize(size_ - position);
		}
		else {
				std::advance(outsideBegin_, position);
				if (isSizeSet_)
					setSize(size_ - position);
		}
    }


};

}
