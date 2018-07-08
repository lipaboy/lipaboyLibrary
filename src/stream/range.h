#pragma once

#include "stream.h"

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

// TODO: move RangeType at own file

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
class Stream<StorageInfo, TIterator>::RangeType {
public:
    using ValueType = Stream::ValueType;
    using reference = ValueType&;
    using const_reference = const reference;
    using OutsideIterator = typename Stream::outside_iterator;
    using size_type = Stream::size_type;
    using TIndex = size_type;
    using GeneratorTypePtr = typename Stream::GeneratorTypePtr;
    using OwnContainerType = OwnContainerTypeWithoutValueType<ValueType>;
    using OwnContainerTypePtr = std::unique_ptr<OwnContainerType>;
    using OwnIterator = typename Stream::OwnIterator;

public:
    RangeType(std::initializer_list<T> init)
        : pContainer_(new OwnContainerType(init)), pGenerator_(nullptr)
    {
        size_ = pContainer_->size();
        setOwnIndices(0, pContainer_->size());
    }
    template <class OuterIterator>
    RangeType(OuterIterator begin, OuterIterator end)
        : outsideBegin_(begin), outsideEnd_(end), pGenerator_(nullptr)
    {
        if constexpr (std::is_same<typename std::iterator_traits<OuterIterator>::iterator_category,
                std::input_iterator_tag>::value == false)
            size_ = std::distance(begin, end);
    }
    RangeType(GeneratorTypePtr generator)
        : outsideBegin_(), outsideEnd_(), pGenerator_(generator), size_(0),
          action_([] (RangeType*) {})
    {
        setOwnIndicesByDefault();
    }
    RangeType(const RangeType& obj)
        : outsideBegin_(obj.outsideBegin_), outsideEnd_(obj.outsideEnd_),
          ownBeginIndex_(obj.ownBeginIndex_), ownEndIndex_(obj.ownEndIndex_),
          pContainer_(obj.pContainer_ == nullptr ? nullptr : new OwnContainerType(*obj.pContainer_)),
          pGenerator_(obj.pGenerator_), size_(obj.size_), action_(obj.action_)
    {}
    RangeType(RangeType&& obj)
        : outsideBegin_(std::move(obj.outsideBegin_)), outsideEnd_(std::move(obj.outsideEnd_)),
          ownBeginIndex_(obj.ownBeginIndex_), ownEndIndex_(obj.ownEndIndex_),
          pContainer_(std::move(obj.pContainer_)),
          pGenerator_(std::move(obj.pGenerator_)), size_(obj.size_), action_(std::move(obj.action_))
    {}

public:
    OwnContainerTypePtr makeContainer() { return std::make_unique<OwnContainerType>(); }

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
    typename GetRangeIter<isOwnIterator, RangeType>::TIterator ibegin() const {
        return GetRangeIter<isOwnIterator, RangeType>::begin(*this);
    }
    template <bool isOwnIterator>
    typename GetRangeIter<isOwnIterator, RangeType>::TIterator iend() const {
        return GetRangeIter<isOwnIterator, RangeType>::end(*this);
    }

public:
    template <bool isOwnIterator>
    ValueType get(size_type ind) const {
        auto iter = ibegin<isOwnIterator>();
        std::advance(iter, ind);
        return *iter;
    }

    void setSize(size_type newSize) {
        size_ = newSize;
        if (pContainer_ != nullptr) {
            setOwnIndices(ownBeginIndex_, ownBeginIndex_ + size());
        }
        else {
            outsideEnd_ = outsideBegin();
            std::advance(outsideEnd_, size());
        }
    }
    size_type size() const { return size_; }
    bool isInfinite() const { return (pGenerator_ != nullptr && pContainer_ == nullptr); }
    void setContainer(OwnContainerTypePtr pNewContainer) {
        pContainer_ = std::move(pNewContainer);
        setOwnIndicesByDefault();
        setSize(pContainer_->size());
    }
    void makeFinite(size_type size) {
        setSize(size);
        if (isInfinite()) {
            pContainer_ = makeContainer();
            // You can't make it only for this because action_ may be copied at another range object
            setAction([] (RangeType * obj) -> void {
                for (size_type i = 0; i < obj->size(); i++)
                    obj->pContainer_->push_back(obj->pGenerator_());
                obj->setOwnIndicesByDefault();
                obj->setAction([] (RangeType*) {});  // Why we can use private property in lambda?
            });
        }
    }
    void copyToOwnContainer(size_type size) {
        auto pNewContainer = makeContainer();
        if (pContainer_ == nullptr) {
            auto iter = outsideBegin();
            for (size_type i = 0; i < size; i++)
                pNewContainer.push_back(*(iter++));
        }
        else {
            auto iter = ownBegin();
            for (size_type i = 0; i < size; i++)
                pNewContainer.push_back(std::move(*(iter++)));
        }
        pContainer_ = std::move(pNewContainer);
    }

    void doPreliminaryActions() { action_(this); }
    void setAction(std::function<void(RangeType*)> newAction) { action_ = newAction; }

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
    void setOwnIndicesByDefault() {
        ownBeginIndex_ = 0;
        ownEndIndex_ = size();
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
    size_type size_;     // think about skip(count) before get(border)
    std::function<void(RangeType*)> action_ = [] (RangeType*) {};

public:
    template <bool isOwnIterator_>
    void moveBeginIter(size_type position) {
        if constexpr (isOwnIterator_) {
            setOwnIndices(ownBeginIndex_ + position, ownEndIndex_);
            setSize(ownEndIndex_ - ownBeginIndex_);
        }
        else {
            std::advance(outsideBegin_, position);
            setSize(size() - position);
        }
    }
};

}
