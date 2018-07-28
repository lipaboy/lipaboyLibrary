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
using lipaboy_lib::RelativeForward;


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
    Stream(GeneratorTypePtr generator) : range_(generator) {}

    //----------------------Methods API-----------------------//

    // TODO: put off this methods into "global" function operators (for move-semantics of *this)

    auto operator| (get functor) -> ExtendedStreamType<get> {
        using ExtendedStream = ExtendedStreamType<get>;
        ExtendedStream newStream(functor, *this);
        newStream.preAction_ =              // preAction_ -> is important
                                            // (because before generating the elements you must set the size)
            [] (ExtendedStream * obj)
            {
                auto border = obj->operation().border();
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
            obj->range().template moveBeginIter<ExtendedStream::isOwnContainer()>(obj->operation().index());
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
    template <class Stream_, class Accumulator, class IdentityFn>
    auto apply(Stream_ & obj, reduce<Accumulator, IdentityFn> const & reduceObj)
        -> typename reduce<Accumulator, IdentityFn>::IdentityRetType
    {
        using RetType = typename reduce<Accumulator, IdentityFn>::IdentityRetType;
        obj.initSlider();
        if (obj.hasNext()) {
            auto result = reduceObj.identity(obj.nextElem());
            for (; obj.hasNext(); )
                result = reduceObj.accum(result, obj.nextElem());
            return result;
        }
        return RetType();
    }

public:
    std::ostream& operator| (print_to&& printer) {
        return apply(*this, std::move(printer));
    }

    template <class Accumulator, class IdentityFn>
    auto operator| (reduce<Accumulator, IdentityFn> const & reduceObj)
        -> typename reduce<Accumulator, IdentityFn>::IdentityRetType
    {
        return apply(*this, reduceObj);
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
    ResultValueType nextElem() { return std::move(nextElem<isOwnContainer()>()); }
    template <bool isOwnContainer_>
    ResultValueType nextElem() { return std::move(range().template nextElem<isOwnContainer_>()); }

    // Why can't we realize currentElem() interface:
    // 1) problem with group operations: somebody must storage the result
    //    of getting current element. Okey, we have tempValue.
    //    But who will initialize it? nextElem()? initSlider()?
    //    Or maybe add condition on first getting current element?
    //    But condition is bad because conveyor breaks down on it.
    ValueType currentElem() { return std::move(currentElem<isOwnContainer()>()); }
    template <bool isOwnContainer_>
    ValueType currentElem() { return std::move(range().template currentElem<isOwnContainer_>()); }

    ValueType currentAtom() const { return std::move(currentAtom<isOwnContainer()>()); }
    template <bool isOwnContainer_>
    ValueType currentAtom() const { return std::move(range().template currentElem<isOwnContainer_>()); }

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
};


//--------------------------------------------------------------------------//
//------------------Extending stream by concating operations-----------------//
//--------------------------------------------------------------------------//


template <class TStream, class Transform>
auto operator| (TStream&& stream, map<Transform> functor)
    -> shortening::StreamTypeExtender<TStream, map<Transform> >
{
#ifdef LOL_DEBUG_NOISY
    cout << "---Add Map---" << endl;
#endif
    return shortening::StreamTypeExtender<TStream, map<Transform> >(functor, std::forward<TStream>(stream));
}


template <class TStream, class Predicate>
auto operator| (TStream&& stream, filter<Predicate> functor)
    -> shortening::StreamTypeExtender<TStream, filter<Predicate> >
{
    using ExtendedStream = shortening::StreamTypeExtender<TStream, filter<Predicate> >;
    return //ExtendedStream::setFilterAction(
                ExtendedStream(functor, std::forward<TStream>(stream)
                               //)
                );
}


}
