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
	// TODO: remove this crutch (Visual Studio)
	using GeneratorTypePtr = //typename Stream::GeneneratorTypePtr;
		std::function<ValueType(void)>;
    using OwnContainerType = OwnContainerTypeWithoutValueType<ValueType>;
    using OwnContainerTypePtr = std::unique_ptr<OwnContainerType>;
    using OwnIterator = typename Stream::OwnIterator;

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
          action_([] (Range*) {})
    {
        setOwnIndices(0, 0);
    }
    Range(const Range& obj)
        : outsideBegin_(obj.outsideBegin_), outsideEnd_(obj.outsideEnd_),
          ownBeginIndex_(obj.ownBeginIndex_), ownEndIndex_(obj.ownEndIndex_),
          pContainer_(obj.pContainer_ == nullptr ? nullptr : new OwnContainerType(*obj.pContainer_)),
          pGenerator_(obj.pGenerator_), action_(obj.action_)
    {}
    Range(Range&& obj) noexcept
        : outsideBegin_(std::move(obj.outsideBegin_)), outsideEnd_(std::move(obj.outsideEnd_)),
          ownBeginIndex_(obj.ownBeginIndex_), ownEndIndex_(obj.ownEndIndex_),
          pContainer_(std::move(obj.pContainer_)),
          pGenerator_(std::move(obj.pGenerator_)), action_(std::move(obj.action_))
    {}

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

    // Moves end-iter (and copies data to own container if Range used outer one)
    void setSize(size_type newSize) {
        if constexpr (StorageInfo::info == OUTSIDE_ITERATORS) {
                if (pContainer_ == nullptr) {
                    pContainer_ = makeContainer(outsideBegin(), outsideEnd());
                    setOwnIndices(0, pContainer_->size());
                }
        }
        // move end-iter
        if (newSize <= ownEndIndex_ - ownBeginIndex_)
            setOwnIndices(ownBeginIndex_, ownBeginIndex_ + newSize);
    }
    //size_type size() const { return size_; }
    bool isInfinite() const { return (pGenerator_ != nullptr && pContainer_ == nullptr); }
    void setContainer(OwnContainerTypePtr pNewContainer) {
        pContainer_ = std::move(pNewContainer);
        setOwnIndices(0, pContainer_->size());
    }
    void makeFinite(size_type size) {
        if (isInfinite()) {
            // You can't make it only for this because action_ may be copied at another range object
            setAction([size] (Range * obj) -> void {
                obj->pContainer_ = obj->makeContainer();
                for (size_type i = 0; i < size; i++)
                    obj->pContainer_->push_back(obj->pGenerator_());
                obj->setOwnIndices(0, obj->pContainer_->size());
                obj->setAction([] (Range*) {});  // Why we can use private property in lambda?
            });
        }
    }

    void doPreliminaryActions() { action_(this); }
    void setAction(std::function<void(Range*)> newAction) { action_ = newAction; }

    //-----------------Slider API---------------//

    template <bool isOwnIterator>
    void initSlider() {
        if constexpr (isOwnIterator)
                ownIterSlider_ = ownBegin();
        else
                outsideIterSlider_ = outsideBegin();
    }
    template <bool isOwnIterator>
    ValueType nextElem() {
        if constexpr (isOwnIterator)
                return *(ownIterSlider_++);
        else {
                ValueType value = *(outsideIterSlider_);
                // Note: you can't optimize it because for istreambuf_iterator
                //       post-increment operator has unspecified by standard
                ++outsideIterSlider_;
                return std::move(value);
        }
    }
    template <bool isOwnIterator>
    ValueType currentElem() const {
        if constexpr (isOwnIterator)
                return *ownIterSlider_;
        else {
                return *outsideIterSlider_;
        }
    }
    template <bool isOwnIterator>
    bool hasNext() const {
        if constexpr (isOwnIterator)
                return (ownIterSlider_ != ownEnd());
        else
                return (outsideIterSlider_ != outsideEnd());
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
        else
                return (ownBeginIndex_ == other.ownBeginIndex_
                        && ownEndIndex_ == other.ownEndIndex_
                        && pContainer_ == other.pContainer_
                        );
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

    // Generator
    GeneratorTypePtr pGenerator_;
    std::function<void(Range*)> action_ = [] (Range*) {};

public:
    template <bool isOwnIterator_>
    void moveBeginIter(size_type position) {
        if constexpr (isOwnIterator_) {
            setOwnIndices(ownBeginIndex_ + position, ownEndIndex_);
            setSize(ownEndIndex_ - ownBeginIndex_);
        }
        else {
            std::advance(outsideBegin_, position);
        }
    }
};

}
