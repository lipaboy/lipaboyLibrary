#pragma once

#include "stream_extended.h"
#include "extra_tools/extra_tools.h"

#include <vector>
#include <functional>
#include <algorithm>
#include <iterator>
#include <type_traits>

#include <typeinfo>

#include <iostream>

namespace stream_space {

using std::vector;
using std::pair;
using std::string;

using std::cout;
using std::endl;
using lipaboy_lib::relativeForward;


//--------------------------Stream Base (specialization class)----------------------//

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
    class Range;
    using OwnContainerType = typename Range::OwnContainerType;
    using OwnContainerTypePtr = typename Range::OwnContainerTypePtr;   // TODO: make it unique (it is not easy)
    using OwnIterator = typename OwnContainerType::iterator;

    // TODO: make unique_ptr
    using GeneratorTypePtr = std::function<T(void)>;

    template <class Functor>
    using ExtendedStreamType = Stream<Functor, StorageInfo, OutsideIterator>;

    using ResultValueType = ValueType;

    template <typename, typename, typename...> friend class Stream;

public:
    //----------------------Constructors----------------------//

    template <class OuterIterator>
    explicit
    Stream(OuterIterator begin, OuterIterator end) : range_(begin, end) {}
    explicit
    Stream(std::initializer_list<T> init) : range_(init) {}
//    Stream(const Stream& obj) : range_(obj.range_) {}
//    Stream(Stream&& obj) noexcept : range_(std::move(obj.range_)) {}
    template <class StreamBase_>
    Stream(StreamBase_&& obj,
           std::enable_if_t<std::is_same_v<Stream, StreamBase_>, int*> p = nullptr) noexcept
        : range_(obj.range_) {}
    explicit
    Stream(GeneratorTypePtr generator) : range_(generator) {}

    //----------------------Methods API-----------------------//

    // TODO: put off this methods into "global" function operators (for move-semantics of *this)

    template <class Predicate>
    auto operator| (filter<Predicate> functor) -> ExtendedStreamType<filter<Predicate> >
    {
        using ExtendedStream = ExtendedStreamType<filter<Predicate> >;
        ExtendedStream newStream(functor, *this);
        // you can't constraint the lambda only for this because the object will be changed after moving
        newStream.action_ = [] (ExtendedStream* obj) {
            obj->throwOnInfiniteStream();
            obj->filter_(obj->getFunctor().functor());
            obj->action_ = [] (ExtendedStream*) {};
        };
        return std::move(newStream);
    }
    template <class Transform>
    auto operator| (map<Transform> functor) -> ExtendedStreamType<map<Transform> > {
        ExtendedStreamType<map<Transform> > newStream(functor, *this);
        return std::move(newStream);
    }
    auto operator| (get functor) -> ExtendedStreamType<get> {
        using ExtendedStream = ExtendedStreamType<get>;
        ExtendedStream newStream(functor, *this);
        newStream.preAction_ =              // preAction_ -> is important
                                            // (because before generating the elements you must set the size)
            [] (ExtendedStream * obj)
            {
                auto border = obj->getFunctor().border();
                if (obj->range().isInfinite())
                    obj->range().makeFinite(border);
                else
                    obj->range().setSize(border);
                obj->preAction_ = [] (ExtendedStream*) {};
            };
        return std::move(newStream);
    }
    auto operator| (group_by_vector functor) -> ExtendedStreamType<group_by_vector> {
        ExtendedStreamType<group_by_vector> newStream(functor, *this);
        return std::move(newStream);   // copy container (only once)
    }
    auto operator| (skip&& skipObj) -> ExtendedStreamType<skip> {
        using ExtendedStream = ExtendedStreamType<skip>;
        ExtendedStream newStream(skipObj, *this);
        newStream.action_ = [] (ExtendedStream* obj) {
            obj->range().template moveBeginIter<ExtendedStream::isOwnContainer()>(obj->functor_.index());
            obj->action_ = [] (ExtendedStream*) {};
        };
        return std::move(newStream);
    }

    auto operator| (ungroupByBit functor) -> ExtendedStreamType<ungroupByBit> {
        ExtendedStreamType<ungroupByBit> newStream(functor, *this);
        return std::move(newStream);
    }

    //-----------Terminated operations------------//
protected:
    template <class Stream_>
    std::ostream& apply(Stream_ & obj, print_to&& printer) {
        obj.doPreliminaryActions();
        for (obj.initSlider(); obj.hasNext(); )
            printer.ostream() << obj.nextElem() << printer.delimiter();
        return printer.ostream();
    }
    template <class Stream_>
    vector<typename Stream_::ResultValueType> apply(Stream_ & obj, to_vector&&)
    {
        using ToVectorType = vector<typename Stream_::ResultValueType>;
        ToVectorType toVector;
        for (obj.initSlider(); obj.hasNext(); )
            toVector.push_back(obj.nextElem());
        return std::move(toVector);
    }

public:
    std::ostream& operator| (print_to&& printer) {
        return apply(*this, std::move(printer));
    }

    ResultValueType operator| (sum&&) {
        initSlider();
        if (hasNext()) {
            auto result = nextElem();
            for (; hasNext();)
                result += nextElem();
            return result;
        }
        return ResultValueType();
    }
    ResultValueType operator| (nth&& nthObj) {
        initSlider();
        for (size_type i = 0; i < nthObj.index() && hasNext(); i++)
            nextElem();
        return nextElem();
    }
    vector<ValueType> operator| (to_vector&& toVectorObj) {
        return apply(*this, std::move(toVectorObj));
    }

    //------------------Additional methods---------------//

//    size_type size() const { return range_.size(); }

public:
    Range & range() { return range_; }
    const Range & range() const { return range_; }
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

    void throwOnInfiniteStream() const {
        if (range().isInfinite())
            throw std::logic_error("Infinite stream");
    }
protected:
    ValueType getElem(size_type index) const { return getElem<isOwnContainer()>(index); }
    template <bool isOwnContainer_>
    ValueType getElem(size_type index) const {
        return this->range().template get<isOwnContainer_>(index);
    }

    //-----------------Slider API--------------//

    void initSlider() { initSlider<isOwnContainer()>(); }
    template <bool isOwnContainer_>
    void initSlider() {
        doPreliminaryActions();
        throwOnInfiniteStream();
        range().template initSlider<isOwnContainer_>();
    }
private:
    void doPreliminaryActions() { range().doPreliminaryActions(); }

protected:
    ResultValueType nextElem() { return nextElem<isOwnContainer()>(); }
    template <bool isOwnContainer_>
    ResultValueType nextElem() { return range().template nextElem<isOwnContainer_>(); }

    // Why can't we realize this interface:
    // 1) problem with group operations: somebody must storage the result
    //    of getting current element. Okey, we have tempValue.
    //    But who will initialize it? nextElem()? initSlider()?
    //    Or maybe add condition on first getting current element?
    //    But condition is bad because conveyor breaks down on it.
//    ValueType currentElem() { return currentElem<isOwnContainer()>(); }
//    template <bool isOwnContainer_>
//    ValueType currentElem() { return range().template currentElem<isOwnContainer_>(); }

    ValueType currentAtom() const { return currentAtom<isOwnContainer()>(); }
    template <bool isOwnContainer_>
    ValueType currentAtom() const { return range().template currentElem<isOwnContainer_>(); }

    bool hasNext() const { return hasNext<isOwnContainer()>(); }
    template <bool isOwnContainer_>
    bool hasNext() const { return range().template hasNext<isOwnContainer_>(); }

    //-----------------Slider API Ends--------------//

public:
    bool operator==(Stream & other) { return equals(other); }
    bool operator!=(Stream & other) { return !((*this) == other); }
private:
    bool equals(Stream & other) { return range_.equals(other.range_); }

private:
    Range range_;

public:
    template <class Accumulator, class IdenityFn>
    auto operator| (reduce<Accumulator, IdenityFn> const & reduceObj)
        -> decltype(auto)//typename reduce<Accumulator, IdenityFn>::
            //template IdentityRetType<ResultValueType>::type
    {
        return apply(*this, reduceObj);
    }

private:
    template <class Stream_, class Accumulator, class IdenityFn>
    auto apply(Stream_ & obj, reduce<Accumulator, IdenityFn> const & reduceObj)
        -> decltype(auto)
    {
        using RetType = typename std::remove_reference<decltype(reduceObj.identity(obj.nextElem()))>::type;
        obj.initSlider();
        if (obj.hasNext()) {
            auto result = reduceObj.identity(obj.nextElem());
            for (; obj.hasNext(); )
                result = reduceObj.accum(result, obj.nextElem());
            return result;
        }
        return RetType();
    }
};


//------------------KEK-----------------//


// REMINDER: don't forget change friend-declaration into StreamExtended
template <class TStream, class TMap>
auto addMap (TStream stream, TMap functor)
    -> typename TStream::template ExtendedStreamType<std::remove_reference_t<TMap> >
{
    typename TStream::template ExtendedStreamType<std::remove_reference_t<TMap> >
            newStream(functor,
                      std::forward<TStream>(
                          stream
                        )
                      );
    return std::move(newStream);
}




}
