#pragma once

#include "operations/operations.h"

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
        operation_(obj.operation_)
    {
#ifdef LOL_DEBUG_NOISY
        cout << "   StreamEx copy-constructed" << endl;
#endif
    }
    Stream (Stream&& obj) noexcept
        : SuperType(std::move(obj)),
        operation_(std::move(obj.operation_))
    {
#ifdef LOL_DEBUG_NOISY
        cout << "   StreamEx move-constructed" << endl;
#endif
    }

	//----------------------Methods API-----------------------//

            //-----------------Tools-------------------//
protected:
    static constexpr bool isNoGetTypeBefore() {
        return (TOperation::metaInfo != GET && SuperType::isNoGetTypeBefore());
    }
    static constexpr bool isGeneratorProducing() {
        return SuperType::isGeneratorProducing();
    }

public:
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
		return std::move(operation_.nextElem<SuperType>(*superThisPtr()));
    }

    // TODO: must be test
	ResultValueType currentElem() {
		// #Crutch: it is strange crutch because Visual Studio can't call the template method 
		// without argument
		// that can help it to deduce the type of template method
		// Solve: Problem in the intersection 
		//        of names (currentElem() of Stream and currentElem() of operation_)
		return std::move(operation_.currentElem<SuperType>(*superThisPtr()));
    }

    bool hasNext() {
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

};

}

}
