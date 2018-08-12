#pragma once

#include "operations.h"

#include <vector>
#include <functional>
#include <algorithm>
#include <iterator>
#include <type_traits>
#include <memory>

#include <typeinfo>

#include <iostream>

namespace lipaboy_lib {

namespace stream_space {

using std::vector;
using std::pair;
using std::string;
using std::unique_ptr;
using std::shared_ptr;

using namespace operations_space;

using std::cout;
using std::endl;

//-----------------Stream Extended class----------------------//

template <class TOperation, class... Rest>
class Stream : public Stream<Rest...> {
public:
    using size_type = size_t;
    using OperationType = TOperation;
    using SuperType = Stream<Rest...>;
    using ConstSuperType = const SuperType;
    using iterator = typename SuperType::outside_iterator;
    using SuperTypePtr = SuperType *;
    using ConstSuperTypePtr = const SuperType *;
    using ValueType = typename SuperType::ValueType;
    using Range = typename SuperType::Range;
    using ActionSignature = void(Stream*);
    using ActionType = std::function<ActionSignature>;

    template <class Functor>
    using ExtendedStreamType = Stream<Functor, OperationType, Rest...>;

    using ResultValueType = typename TOperation::template RetType<typename SuperType::ResultValueType>;

    template <typename, typename...> friend class Stream;

    //----------------------Constructors----------------------//
public:

    template <class StreamSuperType_, class TFunctor_>
    explicit
    Stream (TFunctor_&& functor, StreamSuperType_&& obj) noexcept
        : SuperType(std::forward<StreamSuperType_>(obj)), operation_(std::forward<TFunctor_>(functor))
    {
		if constexpr (TOperation::metaInfo == UNGROUP_BY_BIT) {
				ungroupTempOwner_ = std::make_shared<UngroupTempValueType>();
				ungroupTempOwner_->indexIter = 0;
		}
#ifdef LOL_DEBUG_NOISY
        if constexpr (std::is_rvalue_reference<StreamSuperType_&&>::value)
                cout << "   Stream is extended by move-constructor" << endl;
        else
                cout << "   Stream is extended by copy-constructor" << endl;
#endif
    }
public:
    Stream (Stream const & obj)
        : SuperType(static_cast<ConstSuperType&>(obj)),
        operation_(obj.operation_),
        action_(obj.action_),
		ungroupTempOwner_(obj.ungroupTempOwner_)
    {
#ifdef LOL_DEBUG_NOISY
        cout << "   StreamEx copy-constructed" << endl;
#endif
    }
    Stream (Stream&& obj) noexcept
        : SuperType(std::move(obj)),
        operation_(std::move(obj.operation_)),
        action_(std::move(obj.action_)),
		ungroupTempOwner_(std::move(obj.ungroupTempOwner_))
    {
#ifdef LOL_DEBUG_NOISY
        cout << "   StreamEx move-constructed" << endl;
#endif
    }

    //----------------------Methods API-----------------------//

    auto operator| (get functor) -> ExtendedStreamType<get> {
        using ExtendedStream = ExtendedStreamType<get>;
        ExtendedStream newStream(functor, *this);
        return std::move(newStream);
    }
    auto operator| (group_by_vector functor) -> ExtendedStreamType<group_by_vector> {
        ExtendedStreamType<group_by_vector> obj(functor, *this);
        return std::move(obj);
    }
    auto operator| (skip&& skipObj) -> ExtendedStreamType<skip> {
        using ExtendedStream = ExtendedStreamType<skip>;
        ExtendedStream newStream(skipObj, *this);
        return std::move(newStream);
    }
    auto operator| (ungroup_by_bit functor) -> ExtendedStreamType<ungroup_by_bit> {
        ExtendedStreamType<ungroup_by_bit> obj(functor, *this);
        return std::move(obj);
    }

    //-------------------Terminated operations-----------------//

    std::ostream& operator| (print_to&& printer) {
		assertOnInfiniteStream<Stream>();
        return apply(*this, std::move(printer));
    }
    template <class Accumulator, class IdenityFn>
    auto operator| (reduce<Accumulator, IdenityFn> const & reduceObj)
        -> typename reduce<Accumulator, IdenityFn>::IdentityRetType
    {
		assertOnInfiniteStream<Stream>();
        return apply(*this, reduceObj);
    }
    ResultValueType operator| (sum&&) {
		assertOnInfiniteStream<Stream>();
        init();
        if (hasNext()) {
            auto result = nextElem();
            for (; hasNext();)
                result += nextElem();
            return result;
        }
        return ResultValueType();
    }
    ResultValueType operator| (nth&& nthObj) {
		assertOnInfiniteStream<Stream>();
		return apply(*this, std::move(nthObj));
    }
    vector<ResultValueType> operator| (to_vector&& toVectorObj) {
		assertOnInfiniteStream<Stream>();
        return apply(*this, std::move(toVectorObj));
    }

            //-----------------Tools-------------------//
protected:
    static constexpr bool isNoGetTypeBefore() {
        return (TOperation::metaInfo != GET && SuperType::isNoGetTypeBefore());
    }
    static constexpr bool isNoGroupBefore() {
        return (TOperation::metaInfo != GROUP_BY_VECTOR && SuperType::isNoGroupBefore());
    }
    static constexpr bool isGeneratorProducing() {
        return SuperType::isGeneratorProducing();
    }
	template <class TStream_>
	inline static constexpr void assertOnInfiniteStream() { 
		SuperType::template assertOnInfiniteStream<TStream_>(); 
	}

protected:
    // shortness
    SuperTypePtr superThisPtr() { return static_cast<SuperTypePtr>(this); }
    ConstSuperTypePtr constSuperThisPtr() const { return static_cast<ConstSuperTypePtr>(this); }
    auto superNextElem() -> typename SuperType::ResultValueType { return superThisPtr()->nextElem(); }
    bool superHasNext() { return superThisPtr()->hasNext(); }
    auto superCurrentElem() -> typename SuperType::ResultValueType {
        return superThisPtr()->currentElem();
    }

    //------------------------------------------------------------------------//
    //-----------------------------Slider API---------------------------------//
    //------------------------------------------------------------------------//

public:

    // For calling all the actions into stream (must be called into terminated operations
    // or before using slider API)
    void init() {
		superThisPtr()->init();
		operation_.init<SuperType>(*superThisPtr());
    }

public:
    ResultValueType nextElem() {
        if constexpr (TOperation::metaInfo == FILTER) {
                auto currElem = superNextElem();
				// ! calling hasNext() of current StreamType ! in order to skip unfilter elems
				this->hasNext();
                return std::move(currElem);
        }
        else if constexpr (TOperation::metaInfo == UNGROUP_BY_BIT) {
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
				return std::move(operation_.nextElem<SuperType>(*superThisPtr()));
    }

    // TODO: must be test
	ResultValueType currentElem() {
		if constexpr (TOperation::metaInfo == UNGROUP_BY_BIT) {
                size_type & indexIter = ungroupTempOwner_->indexIter;
                ValueType & tempValue = ungroupTempOwner_->tempValue;
                bool res = (tempValue & (1 << indexIter)) >> indexIter;
                return res;
        }
		else if constexpr (TOperation::metaInfo == FILTER) {
				// ! calling hasNext() of current StreamType ! in order to skip unfilter elems
				this->hasNext();	
				return std::move(superThisPtr()->currentElem());
		}
		else {
				// #Crutch: it is strange crutch because Visual Studio can't call the template method 
				// without argument
				// that can help it to deduce the type of template method
				// Solve: Problem in the intersection 
				//        of names (currentElem() of Stream and currentElem() of operation_)
				return std::move(operation_.currentElem<SuperType>(*superThisPtr()));
		}
    }

    bool hasNext() {
		if constexpr (TOperation::metaInfo == UNGROUP_BY_BIT)
				return (ungroupTempOwner_->indexIter != 0)
					|| superHasNext();
		else if constexpr (TOperation::metaInfo == FILTER) {
			// TODO: realize shifting the slider (without creating copy of result object)
				for (; superHasNext(); superNextElem())
                    if (true == operation().functor()(superCurrentElem()))
						return true;
				return false;
		}
        else
                return operation_.hasNext<SuperType>(*superThisPtr());
    }


    //------------------------------------------------------------------------//
    //-----------------------------Slider API Ends----------------------------//
    //------------------------------------------------------------------------//


protected:
    Range & range() { return superThisPtr()->range(); }
    const Range & range() const { return constSuperThisPtr()->range(); }

public:
    TOperation const & operation() const { return operation_; }

	// Not strong condition (because you don't compare the operations
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
    TOperation operation_;
    // TODO: add getter/setter
    // TODO: get rid of preAction_: replace it on constexpr condition in doPreliminaryActions()
    ActionType action_ = [] (Stream*) {};
    // uses for ungroup_by_bits operation
    struct UngroupTempValueType {
        size_type indexIter;
        ValueType tempValue;
    };
    shared_ptr<UngroupTempValueType> ungroupTempOwner_;

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

}
