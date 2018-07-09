#pragma once

#include "functors.h"

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

using namespace functors_space;

using std::cout;
using std::endl;

template <bool isOwnContainer, class TRange>
struct GetRangeIter {};

//-----------------Stream Extended class----------------------//

template <class TFunctor, class StorageInfo, class... Rest>
class Stream : public Stream<StorageInfo, Rest...> {
public:
    using size_type = size_t;
    using FunctorType = TFunctor;
    using SuperType = Stream<StorageInfo, Rest...>;
    using iterator = typename SuperType::outside_iterator;
    using SuperTypePtr = SuperType *;
    using ConstSuperTypePtr = const SuperType *;
    using ValueType = typename SuperType::ValueType;
    using RangeType = typename SuperType::RangeType;
    using OwnContainerType = typename SuperType::OwnContainerType;
    using OwnContainerTypePtr = typename SuperType::OwnContainerTypePtr;
    using ActionSignature = void(Stream*);
    using ActionType = std::function<ActionSignature>;

    template <class Functor>
    struct ExtendedStreamType {
        using type = Stream<Functor, FunctorType, StorageInfo, Rest...>;
    };
    using ResultValueType = typename TFunctor::template RetType<typename SuperType::ResultValueType>::type;

    template <typename, typename, typename...> friend class Stream;

    //----------------------Constructors----------------------//
protected:
    Stream(TFunctor const & functor, SuperType const & obj)
        : SuperType(obj), functor_(functor) {}
public:
    Stream(Stream const & obj)
        : SuperType(static_cast<SuperType>(obj)), functor_(obj.functor_), action_(obj.action_),
          preAction_(obj.preAction_) {}
    Stream(Stream&& obj)
        : SuperType(static_cast<SuperType>(obj)),
          functor_(std::move(obj.functor_)),
          action_(std::move(obj.action_)),
          preAction_(std::move(obj.preAction_)) {}

    //----------------------Methods API-----------------------//

    template <class Functor>
    auto operator| (filter<Functor> functor) -> typename ExtendedStreamType<filter<Functor> >::type {
        using ExtendedStream = typename ExtendedStreamType<filter<Functor> >::type;
        ExtendedStream obj(functor, *this);
        obj.action_ = [] (ExtendedStream* obj) {
            obj->throwOnInfiniteStream();
            obj->filter_(obj->getFunctor().functor());
            obj->action_ = [] (ExtendedStream*) {};
        };
        return std::move(obj);
    }
    template <class Functor>
    auto operator| (map<Functor> functor)
        -> typename ExtendedStreamType<map<Functor> >::type
    {
        typename ExtendedStreamType<map<Functor> >::type obj(functor, *this);
        return std::move(obj);
    }
    auto operator| (get functor) -> typename ExtendedStreamType<get>::type {
        using ExtendedStream = typename ExtendedStreamType<get>::type;
        ExtendedStream newStream(functor, *this);
        typename ExtendedStream::ActionType ExtendedStream::*pAction;

        if constexpr (isNoGetTypeBefore() && isGeneratorProducing())
            pAction = &ExtendedStream::preAction_;
        else
            pAction = &ExtendedStream::action_;

        newStream.*pAction = [] (ExtendedStream * obj) {
            auto border = obj->getFunctor().border();
            if (obj->range().isInfinite())
                obj->range().makeFinite(border);
            else
                obj->range().setSize(border);
            obj->preAction_ = [] (ExtendedStream*) {};
        };;

        return std::move(newStream);
    }
    auto operator| (group_by_vector functor) -> typename ExtendedStreamType<group_by_vector>::type {
        typename ExtendedStreamType<group_by_vector>::type obj(functor, *this);
        return std::move(obj);
    }
    auto operator| (skip&& skipObj) -> typename ExtendedStreamType<skip>::type {
        using ExtendedStream = typename ExtendedStreamType<skip>::type;
        ExtendedStream newStream(skipObj, *this);
        newStream.action_ = [] (ExtendedStream* obj) {
            obj->range().template moveBeginIter<ExtendedStream::isOwnContainer()>(obj->functor_.index());
            obj->action_ = [] (ExtendedStream*) {};
        };
        return std::move(newStream);
    }
    auto operator| (ungroupByBit functor) -> typename ExtendedStreamType<ungroupByBit>::type {
        typename ExtendedStreamType<ungroupByBit>::type obj(functor, *this);
        return std::move(obj);
    }

    //-------------------Terminated operations-----------------//

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
    vector<ResultValueType> operator| (to_vector&& toVectorObj) {
        return apply(*this, std::move(toVectorObj));
    }

protected:
    template <class Stream_>
    std::ostream& apply(Stream_ & obj, print_to&& printer) {
        return static_cast<SuperTypePtr>(this)->apply(obj, std::move(printer));
    }
    template <class Stream_>
    vector<typename Stream_::ResultValueType> apply(Stream_ & obj, to_vector&& toVectorObj) {
        return static_cast<SuperTypePtr>(this)->apply(obj, std::move(toVectorObj));
    }

            //-----------------Tools-------------------//
protected:
    static constexpr bool isOwnContainer() {
        return (TFunctor::metaInfo == FILTER
                || TFunctor::metaInfo == GET
                || SuperType::isOwnContainer());
    }
    static constexpr bool isNoGetTypeBefore() {
        return (TFunctor::metaInfo != GET && SuperType::isNoGetTypeBefore());
    }
    static constexpr bool isNoGroupBefore() {
        return (TFunctor::metaInfo != GROUP_BY_VECTOR && SuperType::isNoGroupBefore());
    }
    static constexpr bool isGeneratorProducing() {
        return SuperType::isGeneratorProducing();
    }

protected:
    void throwOnInfiniteStream() const {
        static_cast<ConstSuperTypePtr>(this)->throwOnInfiniteStream();
    }

    //-----------------Slider API--------------//

    // For calling all the actions into stream (must be called into terminated operations
    // or before using slider API)
    void initSlider() { initSlider<isOwnContainer()>(); }
    template <bool isOwnContainer_>
    void initSlider() {
        doPreliminaryActions();
        throwOnInfiniteStream();
        static_cast<SuperTypePtr>(this)->template initSlider<isOwnContainer_>();
        if constexpr (TFunctor::metaInfo == UNGROUP_BY_BIT)
            indexIter = 0;
    }
private:
    // nobody instead of initSlider can call it
    void doPreliminaryActions() {
        preAction_(this);
        static_cast<SuperTypePtr>(this)->doPreliminaryActions();
        action_(this);
    }

protected:
    ResultValueType nextElem() { return nextElem<isOwnContainer()>(); }
    template <bool isOwnContainer_>
    ResultValueType nextElem() {
        // Template Parameter Explaination:
        // must be so (isOwnContainer_ instead of isOwnContainer())
        // because client who call this
        // method and derived from that class may have the another value of isOwnContainer()

        if constexpr (TFunctor::metaInfo == MAP)
            return functor_(static_cast<SuperTypePtr>(this)->template nextElem<isOwnContainer_>());
        else if constexpr (TFunctor::metaInfo == GROUP_BY_VECTOR) {
            auto partSize = functor_.partSize();
            ResultValueType part;
            size_type i = 0;
            for (; i < partSize && hasNext(); i++)
                part.push_back(static_cast<SuperTypePtr>(this)->template nextElem<isOwnContainer_>());

            return std::move(part);
        }
        else if constexpr (TFunctor::metaInfo == UNGROUP_BY_BIT) {
            const size_type bitsCountOfType = 8 * sizeof(ValueType);
            if (indexIter % bitsCountOfType == 0)
                tempValue = static_cast<SuperTypePtr>(this)->template nextElem<isOwnContainer_>();
            bool res = (tempValue & (1 << indexIter)) >> indexIter;
            indexIter = (indexIter + 1) % bitsCountOfType;
            return res;
        }
        else
            return static_cast<SuperTypePtr>(this)->template nextElem<isOwnContainer_>();
    }

    ValueType currentAtom() const { return currentAtom<isOwnContainer()>(); }
    template <bool isOwnContainer_>
    ValueType currentAtom() const {
        return static_cast<ConstSuperTypePtr>(this)->template currentAtom<isOwnContainer_>();
    }

    bool hasNext() const { return hasNext<isOwnContainer()>(); }
    template <bool isOwnContainer_>
    bool hasNext() const {
        if constexpr (TFunctor::metaInfo == UNGROUP_BY_BIT)
            return (indexIter != 0)
                || static_cast<ConstSuperTypePtr>(this)->template hasNext<isOwnContainer_>();
        else
            return static_cast<ConstSuperTypePtr>(this)->template hasNext<isOwnContainer_>();
    }

    //-----------------Slider API Ends--------------//

protected:


    ValueType getContainerElement(size_type index) {
        return applyFunctors(range().template get<isOwnContainer()>(index));
    }
    RangeType & range() { return static_cast<SuperTypePtr>(this)->range(); }
    const RangeType & range() const { return static_cast<ConstSuperTypePtr>(this)->range(); }

public:
//    size_type size() const {
//        if constexpr (TFunctor::metaInfo == GROUP)
//            return (static_cast<ConstSuperTypePtr>(this)->size() + functor_.partSize() - 1) / functor_.partSize();
//        else
//            return static_cast<ConstSuperTypePtr>(this)->size();
//    }
    TFunctor const & getFunctor() const { return functor_; }

protected:
    TFunctor functor_;
    // TODO: add getter/setter
    // TODO: get rid of preAction_: replace it on constexpr condition in doPreliminaryActions()
    ActionType action_ = [] (Stream*) {};
    ActionType preAction_ = [] (Stream*) {};
    // TODO: think about allocating the memory under these vars instead of storaging on stack
    // only for ungroupByBit and group operations
    size_type indexIter;
    typename SuperType::ResultValueType tempValue;

protected:
    template <class TFilterFunctor>
    void filter_(TFilterFunctor functor) {
        if constexpr (TFunctor::metaInfo == FILTER) {
            OwnContainerTypePtr pNewContainer = range().makeContainer();
            SuperTypePtr superThis = static_cast<SuperTypePtr>(this);
            superThis->initSlider();
            for (; superThis->hasNext(); )
            {
                ValueType atom = superThis->currentAtom();
                auto elem = superThis->nextElem();
                // TODO: move the element (only if copy from your own container)
                if (functor(std::move(elem)) == true) {
                    pNewContainer->push_back(atom);
                }
            }
            range().setContainer(std::move(pNewContainer));
        }
    }

public:
    template <class Stream_, class Accumulator, class IdenityFn>
    auto apply(Stream_ & obj, reduce<Accumulator, IdenityFn> const & reduceObj)
        -> decltype(auto)
    {
        return static_cast<SuperTypePtr>(this)->apply(obj, reduceObj);
    }
    template <class Accumulator, class IdenityFn>
    auto operator| (reduce<Accumulator, IdenityFn> const & reduceObj)
        -> decltype(auto) //typename reduce<Accumulator, IdenityFn>::
            //template IdentityRetType<ResultValueType>::type
    {
        return apply(*this, reduceObj);
    }
};

}
