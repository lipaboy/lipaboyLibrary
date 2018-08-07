#pragma once

#include "functors.h"
#include "extra_tools/extra_tools.h"

#include <vector>
#include <functional>
#include <algorithm>
#include <iterator>
#include <type_traits>
#include <memory>

#include <typeinfo>

#include <iostream>

namespace stream_space {

using std::vector;
using std::pair;
using std::string;
using std::unique_ptr;

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
    using ConstSuperType = const SuperType;
    using iterator = typename SuperType::outside_iterator;
    using SuperTypePtr = SuperType *;
    using ConstSuperTypePtr = const SuperType *;
    using ValueType = typename SuperType::ValueType;
    using Range = typename SuperType::Range;
    using OwnContainerType = typename SuperType::OwnContainerType;
    using OwnContainerTypePtr = typename SuperType::OwnContainerTypePtr;
    using ActionSignature = void(Stream*);
    using ActionType = std::function<ActionSignature>;

    template <class Functor>
    using ExtendedStreamType = Stream<Functor, FunctorType, StorageInfo, Rest...>;

    using ResultValueType = typename TFunctor::template RetType<typename SuperType::ResultValueType>::type;

    template <typename, typename, typename...> friend class Stream;

    //----------------------Constructors----------------------//
public:

    template <class StreamSuperType_, class TFunctor_>
    Stream (TFunctor_&& functor, StreamSuperType_&& obj) noexcept
        : SuperType(std::forward<StreamSuperType_>(obj)), operation_(std::forward<TFunctor_>(functor))
    {
        if constexpr (TFunctor::metaInfo == UNGROUP_BY_BIT)
                ungroupTempOwner_ = std::make_unique<UngroupTempValueType>();
        if constexpr (TFunctor::metaInfo == GROUP_BY_VECTOR)
                groupedTempOwner_ = std::make_unique<GroupedTempValueType>();
#ifdef LOL_DEBUG_NOISY
        if constexpr (std::is_rvalue_reference<StreamSuperType_&&>::value)
                cout << "   Stream is extended by move-constructor" << endl;
        else
                cout << "   Stream is extended by copy-constructor" << endl;
#endif
    }
public:
    Stream (Stream const & obj)
        : SuperType(static_cast<ConstSuperType&>(obj))
        ,
          operation_(obj.operation_),
          action_(obj.action_),
          preAction_(obj.preAction_)
    {
        if constexpr (TFunctor::metaInfo == UNGROUP_BY_BIT)
                ungroupTempOwner_ = std::make_unique<UngroupTempValueType>();
        if constexpr (TFunctor::metaInfo == GROUP_BY_VECTOR)
                groupedTempOwner_ = std::make_unique<GroupedTempValueType>();
#ifdef LOL_DEBUG_NOISY
        cout << "   StreamEx copy-constructed" << endl;
#endif
    }
    Stream (Stream&& obj) noexcept
        : SuperType(std::move(obj)),
          operation_(std::move(obj.operation_)),
          action_(std::move(obj.action_)),
          preAction_(std::move(obj.preAction_))
    {
        if constexpr (TFunctor::metaInfo == UNGROUP_BY_BIT)
                ungroupTempOwner_ = std::make_unique<UngroupTempValueType>();
        if constexpr (TFunctor::metaInfo == GROUP_BY_VECTOR)
                groupedTempOwner_ = std::make_unique<GroupedTempValueType>();
#ifdef LOL_DEBUG_NOISY
        cout << "   StreamEx move-constructed" << endl;
#endif
    }

    //----------------------Methods API-----------------------//

    auto operator| (get functor) -> ExtendedStreamType<get> {
        using ExtendedStream = ExtendedStreamType<get>;
        ExtendedStream newStream(functor, *this);
        typename ExtendedStream::ActionType ExtendedStream::*pAction;

        if constexpr (isNoGetTypeBefore() && isGeneratorProducing())
            pAction = &ExtendedStream::preAction_;
        else
            pAction = &ExtendedStream::action_;

        newStream.*pAction = [] (ExtendedStream * obj) {
            auto border = obj->operation().border();
            obj->range().makeFinite(border);
            obj->preAction_ = [] (ExtendedStream*) {};
        };;

        return std::move(newStream);
    }
    auto operator| (group_by_vector functor) -> ExtendedStreamType<group_by_vector> {
        ExtendedStreamType<group_by_vector> obj(functor, *this);
        return std::move(obj);
    }
    auto operator| (skip&& skipObj) -> ExtendedStreamType<skip> {
        using ExtendedStream = ExtendedStreamType<skip>;
        ExtendedStream newStream(skipObj, *this);
        newStream.action_ = [] (ExtendedStream* obj) {
            obj->range().moveBeginIter(obj->operation().index());
            obj->action_ = [] (ExtendedStream*) {};
        };
        return std::move(newStream);
    }
    auto operator| (ungroupByBit functor) -> ExtendedStreamType<ungroupByBit> {
        ExtendedStreamType<ungroupByBit> obj(functor, *this);
        return std::move(obj);
    }

    //-------------------Terminated operations-----------------//

    std::ostream& operator| (print_to&& printer) {
        return apply(*this, std::move(printer));
    }
    template <class Accumulator, class IdenityFn>
    auto operator| (reduce<Accumulator, IdenityFn> const & reduceObj)
        -> typename reduce<Accumulator, IdenityFn>::IdentityRetType
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
		return apply(*this, std::move(nthObj));
    }
    vector<ResultValueType> operator| (to_vector&& toVectorObj) {
        return apply(*this, std::move(toVectorObj));
    }

protected:
    template <class Stream_>
    std::ostream& apply(Stream_ & obj, print_to&& printer) {
        return superThisPtr()->template apply<Stream_>(obj, std::move(printer));
    }
	template <class Stream_>
	auto apply(Stream_ & obj, nth&& nthObj) -> typename Stream_::ResultValueType {
		return superThisPtr()->template apply<Stream_>(obj, std::move(nthObj));
	}
    template <class Stream_>
    auto apply(Stream_ & obj, to_vector&& toVectorObj) -> vector<typename Stream_::ResultValueType> {
        return superThisPtr()->template apply<Stream_>(obj, std::move(toVectorObj));
    }
    template <class Stream_, class Accumulator, class IdenityFn>
    auto apply(Stream_ & obj, reduce<Accumulator, IdenityFn> const & reduceObj)
        -> typename reduce<Accumulator, IdenityFn>::IdentityRetType
    {
        return superThisPtr()->template apply<Stream_>(obj, reduceObj);
    }


            //-----------------Tools-------------------//
protected:
    static constexpr bool isOwnContainer() {
        return (//TFunctor::metaInfo == FILTER
                //||
                //TFunctor::metaInfo == GET
                //||
                SuperType::isOwnContainer());
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
    // shortness
    void throwOnInfiniteStream() const { constSuperThisPtr()->throwOnInfiniteStream(); }
    SuperTypePtr superThisPtr() { return static_cast<SuperTypePtr>(this); }
    ConstSuperTypePtr constSuperThisPtr() const { return static_cast<ConstSuperTypePtr>(this); }
    auto superNextElem() -> typename SuperType::ResultValueType { return superThisPtr()->nextElem(); }
    bool superHasNext() const { return constSuperThisPtr()->hasNext(); }
    auto superCurrentElem() -> typename SuperType::ResultValueType {
        return superThisPtr()->currentElem();
    }

    //------------------------------------------------------------------------//
    //-----------------------------Slider API---------------------------------//
    //------------------------------------------------------------------------//

    // For calling all the actions into stream (must be called into terminated operations
    // or before using slider API)
    void initSlider() {
        doPreliminaryActions();
        throwOnInfiniteStream();
        superThisPtr()->initSlider();
        if constexpr (TFunctor::metaInfo == UNGROUP_BY_BIT)
                ungroupTempOwner_->indexIter = 0;
        else if constexpr (TFunctor::metaInfo == FILTER) {
                // TODO: realize shifting the slider (without creating copy of result object
                for (; hasNext(); superNextElem())
                    if (true == operation().functor()(superCurrentElem()))
                        break;
        }
        else if constexpr (TFunctor::metaInfo == GROUP_BY_VECTOR) {
            auto partSize = operation_.partSize();
            for (size_type i = 0; i < partSize && hasNext(); i++)
                groupedTempOwner_->tempValue.push_back(superNextElem());
        }
    }
private:
    // nobody instead of initSlider can call it
    void doPreliminaryActions() {
        preAction_(this);
        superThisPtr()->doPreliminaryActions();
        action_(this);
    }

protected:
    ResultValueType nextElem() {
        // Template Parameter Explaination:
        // must be so (isOwnContainer_ instead of isOwnContainer())
        // because client who call this
        // method and derived from that class may have the another value of isOwnContainer()

        if constexpr (TFunctor::metaInfo == MAP)
                return std::move(operation()(superNextElem()));
        if constexpr (TFunctor::metaInfo == FILTER) {
                auto currElem = superNextElem();
                for (; hasNext(); superNextElem()) {
                    auto elem = superCurrentElem();
                    if (true == operation().functor()(elem))
                        break;
                }
                return std::move(currElem);
        }
        else if constexpr (TFunctor::metaInfo == GROUP_BY_VECTOR) {
                auto partSize = operation_.partSize();
                ResultValueType part;
                for (size_type i = 0; i < partSize && superHasNext(); i++)
                    part.push_back(superNextElem());

                std::swap(groupedTempOwner_->tempValue, part);
                return std::move(part);
        }
        else if constexpr (TFunctor::metaInfo == UNGROUP_BY_BIT) {
                constexpr size_type bitsCountOfType = 8 * sizeof(ValueType);
                size_type & indexIter = ungroupTempOwner_->indexIter;
                ValueType & tempValue = ungroupTempOwner_->tempValue;

                if (indexIter % bitsCountOfType == 0)
                    tempValue = superNextElem();
                bool res = (tempValue & (1 << indexIter)) >> indexIter;
                indexIter = (indexIter + 1) % bitsCountOfType;
                return res;
        }
        else
                return std::move(superNextElem());
    }

    // TODO: must be test
    ValueType currentElem() {
        if constexpr (TFunctor::metaInfo == MAP)
                return std::move(operation()(superThisPtr()->currentElem()));
        else if constexpr (TFunctor::metaInfo == GROUP_BY_VECTOR)
                return groupedTempOwner_->tempValue;
        else if constexpr (TFunctor::metaInfo == UNGROUP_BY_BIT) {
                size_type & indexIter = ungroupTempOwner_->indexIter;
                ValueType & tempValue = ungroupTempOwner_->tempValue;
                bool res = (tempValue & (1 << indexIter)) >> indexIter;
                return res;
        }
        else
                return std::move(superThisPtr()->currentElem());
    }

    bool hasNext() const {
        if constexpr (TFunctor::metaInfo == UNGROUP_BY_BIT)
                return (ungroupTempOwner_->indexIter != 0)
                    || constSuperThisPtr()->hasNext();
        else if constexpr (TFunctor::metaInfo == GROUP_BY_VECTOR)
                return (!groupedTempOwner_->tempValue.empty()
                        || constSuperThisPtr()->hasNext());
        else
                return constSuperThisPtr()->hasNext();
    }


    //------------------------------------------------------------------------//
    //-----------------------------Slider API Ends----------------------------//
    //------------------------------------------------------------------------//


protected:

    Range & range() { return superThisPtr()->range(); }
    const Range & range() const { return constSuperThisPtr()->range(); }

public:
    TFunctor const & operation() const { return operation_; }

    bool operator==(Stream & other) { return equals(other); }
    bool operator!=(Stream & other) { return !((*this) == other); }
private:
    bool equals(Stream & other) {
        return (operation_ == other.operation_
                && superThisPtr()->equals(static_cast<SuperType&>(other))
                );
    }

    //---------------------------------------------------//
    //---------------------Fields------------------------//
    //---------------------------------------------------//

protected:
    TFunctor operation_;
    // TODO: add getter/setter
    // TODO: get rid of preAction_: replace it on constexpr condition in doPreliminaryActions()
    ActionType action_ = [] (Stream*) {};
    ActionType preAction_ = [] (Stream*) {};
    // uses for ungroup_by_bits operation
    struct UngroupTempValueType {
        size_type indexIter;
        ValueType tempValue;
    };
    unique_ptr<UngroupTempValueType> ungroupTempOwner_;
    // uses for group_to_vector operation
    struct GroupedTempValueType {
        ResultValueType tempValue;
    };
    unique_ptr<GroupedTempValueType> groupedTempOwner_;

protected:

#ifdef LOL_DEBUG_NOISY
        cout << "---Filter ended---" << endl;
#endif

private:

    //--------------------------------------------------------------------------------//
    //-----------------------------------Friends--------------------------------------//
    //--------------------------------------------------------------------------------//

};

}
