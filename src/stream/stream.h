#ifndef STREAM_H
#define STREAM_H

#include "stream_common.h"

#include <memory>
#include <vector>
#include <functional>
#include <algorithm>
#include <optional>
#include <iterator>
#include <type_traits>
#include <utility>
#include <cmath>
#include <ostream>

#include <typeinfo>

#include <iostream>


#define IS_TEST_RUN229

#ifdef IS_TEST_RUN229
#include <gtest/gtest.h>

namespace stream_tests {
class StreamTest;
}

#endif

namespace stream_space {

using std::vector;
using std::pair;
using std::ostream;
using std::string;
using std::unique_ptr;
using std::shared_ptr;
using std::optional;

using namespace std::placeholders;

using std::cout;
using std::endl;


//--------------------------Stream specialization class----------------------//

namespace {
template <class T> using OwnContainerTypeWithoutValueType = vector<T>;
}

template <class StorageInfo, class OutsideIterator>
class Stream<StorageInfo, OutsideIterator> {
public:
    using T = typename std::iterator_traits<OutsideIterator>::value_type;
    using ValueType = T;
    using size_type = size_t;
    using outside_iterator = OutsideIterator;
    class RangeType;
    using OwnContainerType = typename RangeType::OwnContainerType;
    using OwnContainerTypePtr = typename RangeType::OwnContainerTypePtr;   // TODO: make it unique (it is not easy)
    using OwnIterator = typename OwnContainerType::iterator;

    // TODO: make unique_ptr
    using GeneratorTypePtr = std::function<T(void)>;

    template <class Functor>
    struct ExtendedStreamType {
        using type = Stream<Functor, StorageInfo, OutsideIterator>;
    };
    using ResultValueType = ValueType;

    template <typename, typename, typename...> friend class Stream;

public:
    //----------------------Constructors----------------------//

    template <class OuterIterator>
    Stream(OuterIterator begin, OuterIterator end) : range_(begin, end) {}
    Stream(std::initializer_list<T> init) : range_(init) {}
    Stream(const Stream& obj) : range_(obj.range_) {}
    Stream(Stream&& obj) : range_(std::move(obj.range_)) {}
    Stream(GeneratorTypePtr generator) : range_(generator) {}

    //----------------------Methods API-----------------------//

    // TODO: put off this methods into "global" function operators (for move-semantics of *this)

    template <class Predicate>
    auto operator| (filter<Predicate> functor) -> typename ExtendedStreamType<filter<Predicate> >::type
    {
        using ExtendedStream = typename ExtendedStreamType<filter<Predicate> >::type;
        ExtendedStream newStream(functor, *this);
        // you can't constraint the lambda only for this because the object will be changed after moving
        newStream.action_ = [] (ExtendedStream* obj) {
            if (obj->range().isInfinite())
                throw std::logic_error("Infinite stream");
            obj->filter_(obj->getFunctor().functor());
            obj->action_ = [] (ExtendedStream*) {};
        };
        return std::move(newStream);   // copy container (only once)
    }
    template <class Transform>
    auto operator| (map<Transform> functor) -> typename ExtendedStreamType<map<Transform> >::type {
        typename ExtendedStreamType<map<Transform> >::type newStream(functor, *this);
        return std::move(newStream);   // copy container (only once)
    }
    auto operator| (get functor) -> typename ExtendedStreamType<get>::type {
        using ExtendedStream = typename ExtendedStreamType<get>::type;
        ExtendedStream newStream(functor, *this);
        newStream.preAction_ =              // preAction_ -> is important
                                            // (because before generating the elements you must set the size)
            [] (ExtendedStream * obj)
            {
                auto border = obj->getFunctor().border();
                if (obj->range().isInfinite())
                    obj->range().makeFinite(border);
                else if (border <= obj->size())
                    obj->range().setSize(border);
                obj->preAction_ = [] (ExtendedStream*) {};
            };
        return std::move(newStream);   // copy container (only once)
    }
    auto operator| (group functor) -> typename ExtendedStreamType<group>::type {
        typename ExtendedStreamType<group>::type newStream(functor, *this);
        return std::move(newStream);   // copy container (only once)
    }
    auto operator| (skip&& skipObj) -> typename ExtendedStreamType<skip>::type {
        using ExtendedStream = typename ExtendedStreamType<skip>::type;
        ExtendedStream newStream(skipObj, *this);
        newStream.action_ = [] (ExtendedStream* obj) {
            obj->range().template moveBeginIter<ExtendedStream::isOwnContainer()>(obj->functor_.index());
            obj->action_ = [] (ExtendedStream*) {};
        };
        return std::move(newStream);   // copy container (only once)
    }

    //-----------Terminated operations------------//

    std::ostream& operator| (print_to&& printer) {
        doPreliminaryActions();
        initSlider<isOwnContainer()>();
        for ( ; hasNext<isOwnContainer()>(); ) {
            auto ch = nextElem<isOwnContainer()>();
            printer.ostream() << ch << printer.delimiter();
        }
        return printer.ostream();
    }
    template <class Accumulator, class IdenityFn>
    ResultValueType operator| (reduce<Accumulator, IdenityFn>&& reduceObj) {
        doPreliminaryActions();
        auto result = reduceObj.identity(getElem(0));
        for (size_type i = 1; i < size(); i++)
            result = reduceObj.accum(result, getElem(i));
        return result;
    }
    ResultValueType operator| (sum&&) {
        doPreliminaryActions();
        auto result = getElem(0);
        for (size_type i = 1; i < size(); i++)
            result += getElem(i);
        return result;
    }
    ResultValueType operator| (nth&& nthObj) {
        doPreliminaryActions();
        return getElem(nthObj.index());
    }
    vector<ValueType> operator| (to_vector&&) {
        doPreliminaryActions();
        if (range().isInfinite())
            throw std::logic_error("Infinite stream");
        return vector<ValueType>(range().template ibegin<isOwnContainer()>(),
                                 range().template iend<isOwnContainer()>());;
    }

    //------------------Additional methods---------------//

    size_type size() const { return range_.size(); }

public:
    RangeType & range() { return range_; }
    const RangeType & range() const { return range_; }
protected:
    static constexpr bool isOwnContainer() {
        return StorageInfo::info == INITIALIZING_LIST
                || StorageInfo::info == GENERATOR;
    }
    static constexpr bool isNoGetTypeBefore() {
        return true;
    }
    static constexpr bool isNoGroupBefore() {
        return true;
    }
    static constexpr bool isGeneratorProducing() {
        return StorageInfo::info == GENERATOR;
    }

protected:
    // Info:
    // illusion of protected (it means that can be replace on private)
    // (because all the variadic templates are friends
    // from current Stream to first specialization) (it is not a real inheritance)

    void doPreliminaryActions() { range().doPreliminaryActions(); }
protected:
    ValueType getElem(size_type index) const { return getElem<isOwnContainer()>(index); }
    template <bool isOwnContainer_>
    ValueType getElem(size_type index) const {
        return this->range().template get<isOwnContainer_>(index);
    }

    //-----------------Slider API--------------//

    template <bool isOwnContainer_>
    void initSlider() { range().template initSlider<isOwnContainer_>(); }
    template <bool isOwnContainer_>
    ValueType nextElem() { return range().template nextElem<isOwnContainer_>(); }
    template <bool isOwnContainer_>
    bool hasNext() { return range().template hasNext<isOwnContainer_>(); }

    decltype(auto) bindFunctors() const {
        return std::bind([] (ValueType const & a) -> ValueType const & { return a; }, _1);
    }

private:
    RangeType range_;
};


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
    using OwnContainerTypePtr = unique_ptr<OwnContainerType>;
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

    void doPreliminaryActions() { action_(this); }
    void setAction(std::function<void(RangeType*)> newAction) { action_ = newAction; }

    //-----------------Slider API---------------//

    template <bool isOwnIterator>
    void initSlider() {
        if constexpr (isOwnIterator)
            ownIterSlider = ownBegin();
        else
            outsideIterSlider = outsideBegin();
    }
    template <bool isOwnIterator>
    ValueType nextElem() {
        if constexpr (isOwnIterator)
            return *(ownIterSlider++);
        else {
            ValueType value = *(outsideIterSlider);
            // Note: you can't optimize it because for istreambuf_iterator
            //       post-increment operator has unspecified by standard
            ++outsideIterSlider;
            return std::move(value);
        }
    }
    template <bool isOwnIterator>
    bool hasNext() {
        if constexpr (isOwnIterator)
            return (ownIterSlider != ownEnd());
        else
            return (outsideIterSlider != outsideEnd());
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
    OutsideIterator outsideIterSlider;
    OwnIterator ownIterSlider;

    // Container
    TIndex ownBeginIndex_;
    TIndex ownEndIndex_;
    OwnContainerTypePtr pContainer_ = nullptr;

    // Generator
    GeneratorTypePtr pGenerator_;
    size_type size_;     // think about skip(count) before get(border)
    std::function<void(RangeType*)> action_ = [] (RangeType*) {};

public:
    template <bool isOwnIterator>
    void moveBeginIter(size_type position) {
        if constexpr (isOwnIterator) {
            setOwnIndices(ownBeginIndex_ + position, ownEndIndex_);
            setSize(ownEndIndex_ - ownBeginIndex_);
        }
        else {
            std::advance(outsideBegin_, position);
            setSize(size() - position);
        }
    }
};

//-------------------Wrapper-----------------------//

template <class TIterator>
auto createStream(TIterator begin, TIterator end)
    -> Stream<IsOutsideIteratorsRefer, TIterator>
{
    return Stream<IsOutsideIteratorsRefer, TIterator>(begin, end);
}

template <class T>
decltype(auto) createStream(std::initializer_list<T> init)
{
    return Stream<IsInitializingListCreation, typename OwnContainerTypeWithoutValueType<T>::iterator>(init);
}

template <class T, class... Args>
decltype(auto) createStream(T elem, Args... args)
{
    return Stream<IsInitializingListCreation,
            typename OwnContainerTypeWithoutValueType<T>::iterator>({elem, args...});
}

template <class Generator>
decltype(auto) createStream(Generator&& generator)
{
    return Stream<IsGeneratorProducing,
            typename OwnContainerTypeWithoutValueType<
                typename std::result_of<Generator(void)>::type>::iterator>(std::forward<Generator>(generator));
}

}

#endif // STREAM_H
