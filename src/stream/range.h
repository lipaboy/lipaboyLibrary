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
        : pContainer_(new OwnContainerType(init)), pGenerator_(nullptr)
    {
        setOwnIndices(0, pContainer_->size());
    }
    template <class OuterIterator>
    Range(OuterIterator begin, OuterIterator end)
        : outsideBegin_(begin), outsideEnd_(end), pGenerator_(nullptr)
    {}
	Range(GeneratorTypePtr generator)
		: outsideBegin_(), outsideEnd_(), pGenerator_(generator),
		isInfinite_(true),
		pTempValue_(std::make_shared<ValueType>())
    {
        setOwnIndices(0, 0);
    }

	// TODO : make constructors the default
    Range(const Range& obj)
        : outsideBegin_(obj.outsideBegin_), outsideEnd_(obj.outsideEnd_),
        ownBeginIndex_(obj.ownBeginIndex_), ownEndIndex_(obj.ownEndIndex_),
        pContainer_(obj.pContainer_ == nullptr ? nullptr : new OwnContainerType(*obj.pContainer_)),
        pGenerator_(obj.pGenerator_), 
        isInfinite_(obj.isInfinite_), isSizeSet_(obj.isSizeSet_), size_(obj.size_), 
		pTempValue_(obj.pTempValue_)
    {
#ifdef LOL_DEBUG_NOISY
        cout << " Range copy-constructed" << endl;
#endif
    }
    Range(Range&& obj) noexcept
        : outsideBegin_(std::move(obj.outsideBegin_)), outsideEnd_(std::move(obj.outsideEnd_)),
        ownBeginIndex_(obj.ownBeginIndex_), ownEndIndex_(obj.ownEndIndex_),
        pContainer_(std::move(obj.pContainer_)),
        pGenerator_(std::move(obj.pGenerator_)), 
        isInfinite_(obj.isInfinite_), isSizeSet_(obj.isSizeSet_), size_(obj.size_),
		pTempValue_(std::move(obj.pTempValue_))
    {
#ifdef LOL_DEBUG_NOISY
        cout << " Range move-constructed" << endl;
#endif
    }

public:
    OwnContainerTypePtr makeContainer() { return std::make_unique<OwnContainerType>(); }
    OwnContainerTypePtr makeContainer(size_type size) { return std::make_unique<OwnContainerType>(size); }
    template <class TIterator_>
    OwnContainerTypePtr makeContainer(TIterator_ first, TIterator_ end) {
        return std::make_unique<OwnContainerType>(first, end);
    }

    //------------Iterators API-------------//

    OwnIterator ownBegin() const {
        auto beginIter = pContainer_->begin();
        std::advance(beginIter, ownBeginIndex_);
        return beginIter;
    }
    OwnIterator ownEnd() const {
        auto endIter = pContainer_->begin();
        std::advance(endIter, ownEndIndex_);
        return endIter;
    }
    OutsideIterator outsideBegin() const { return outsideBegin_; }
    OutsideIterator outsideEnd() const { return outsideEnd_; }


    template <bool isOwnIterator>
    typename GetRangeIter<isOwnIterator, Range>::TIterator ibegin() const {
        return GetRangeIter<isOwnIterator, Range>::begin(*this);
    }
    template <bool isOwnIterator>
    typename GetRangeIter<isOwnIterator, Range>::TIterator iend() const {
        return GetRangeIter<isOwnIterator, Range>::end(*this);
    }

public:
    template <bool isOwnIterator>
    ValueType get(size_type ind) const {
        auto iter = ibegin<isOwnIterator>();
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
        else {  // move end-iter
                if (newSize <= ownEndIndex_ - ownBeginIndex_)
                    setOwnIndices(ownBeginIndex_, ownBeginIndex_ + newSize);
        }
    }

    //size_type size() const { return size_; }
    bool isInfinite() const { return isInfinite_; }
    void setContainer(OwnContainerTypePtr pNewContainer) {
        pContainer_ = std::move(pNewContainer);
        setOwnIndices(0, pContainer_->size());
    }
    void makeFinite(size_type size) {
        isInfinite_ = false;
        setSize(size);
    }

    void doPreliminaryActions() {}
    void setAction(std::function<void(Range*)> newAction) { action_ = newAction; }

    //-----------------Slider API---------------//

    template <bool isOwnIterator>
    void initSlider() {
		// !! Problem with double initializing tempValue (when initSlider is called twice -> nth(0); nth(0); )
        if constexpr (StorageInfo::info == GENERATOR)
                *pTempValue_ = pGenerator_();
        else if constexpr (StorageInfo::info == INITIALIZING_LIST)
                ownIterSlider_ = ownBegin();
        else
                outsideIterSlider_ = outsideBegin();
    }
    template <bool isOwnIterator>
    ValueType nextElem() {
        if constexpr (StorageInfo::info == GENERATOR) {
                auto newValue = (size_ > 1) ? pGenerator_() : ValueType();
                std::swap(newValue, *pTempValue_);
                size_ = (hasNext()) ? size_ - 1 : size_;
                return std::move(newValue);
        }
        else if constexpr (StorageInfo::info == INITIALIZING_LIST)
                return *(ownIterSlider_++);
        else {
                ValueType value = *(outsideIterSlider_);
                // Note: you can't optimize it because for istreambuf_iterator
                //       post-increment operator has unspecified by standard
                ++outsideIterSlider_; // maybe replace it to std::next
                size_ = (hasNext()) ? size_ - 1 : size_;
                return std::move(value);
        }
    }
    template <bool isOwnIterator>
    void incrementSlider() {
        if constexpr (StorageInfo::info == GENERATOR) {
                *pTempValue_ = pGenerator_();
                size_ = (hasNext()) ? size_ - 1 : size_;
        }
        else if constexpr (StorageInfo::info == INITIALIZING_LIST)
                ownIterSlider_++;
        else {
                // Note: you can't optimize it because for istreambuf_iterator
                //       post-increment operator has unspecified by standard
                ++outsideIterSlider_;
				size_ = (hasNext()) ? size_ - 1 : size_;
        }
    }
    template <bool isOwnIterator>
    ValueType currentElem() const {
        if constexpr (StorageInfo::info == GENERATOR)
                return *pTempValue_;
        else if constexpr (StorageInfo::info == INITIALIZING_LIST)
                return *ownIterSlider_;
        else
                return *outsideIterSlider_;
    }
    bool hasNext() const {
        if constexpr (StorageInfo::info == GENERATOR)
                return (size_ > 0);
        else if constexpr (StorageInfo::info == INITIALIZING_LIST)
                return (ownIterSlider_ != ownEnd());
        else    // until
                return !(outsideIterSlider_ == outsideEnd() || (isSizeSet_ && size_ <= 0));
    }

protected:
    void setOwnIndices(TIndex first, TIndex second) {
        ownBeginIndex_ = first;
        ownEndIndex_ = second;
    }

public:

    bool equals(Range & other) {
        // INFO: it is the right condition because OwnContainer will be appeared when
        // terminated operation is applied to stream. Before that stream refers to
        // outer iterators (if it uses them from the beginning)
        if constexpr (StorageInfo::info == OUTSIDE_ITERATORS)
                return (outsideBegin() == other.outsideBegin()
                        && outsideEnd() == other.outsideEnd()
                        && pContainer_ == other.pContainer_
                        );
        else if constexpr (StorageInfo::info == INITIALIZING_LIST)
                return (ownBeginIndex_ == other.ownBeginIndex_
                        && ownEndIndex_ == other.ownEndIndex_
                        && pContainer_ == other.pContainer_
                        );
        else
                return (pGenerator_ == other.pGenerator_
                        && size_ == other.size_);
    }

private:
    // Iterators that refer to outside of class container
    OutsideIterator outsideBegin_;
    OutsideIterator outsideEnd_;

    // Slider
    OutsideIterator outsideIterSlider_;
    OwnIterator ownIterSlider_;

    // Container
    TIndex ownBeginIndex_;
    TIndex ownEndIndex_;
    OwnContainerTypePtr pContainer_ = nullptr;
    bool isSizeSet_ = false;
    size_type size_;
    bool isInfinite_ = false;

    // Generator
    GeneratorTypePtr pGenerator_;
	// Note: pTempValue - is something strange (like I don't know
	//		 what's kind of relationship it has with Stream logic)
    shared_ptr<ValueType> pTempValue_ = nullptr;        // only for Generator

public:
    template <bool isOwnIterator_>
    void moveBeginIter(size_type position) {
        if constexpr (isGeneratorProducing())
                for (size_type i = 0; i < position; i++)
                    incrementSlider<isOwnIterator_>();
        else if constexpr (StorageInfo::info == INITIALIZING_LIST) {
                setOwnIndices(ownBeginIndex_ + position, ownEndIndex_);
                setSize(ownEndIndex_ - ownBeginIndex_);
        }
        else
                std::advance(outsideBegin_, position);
    }


};

}
