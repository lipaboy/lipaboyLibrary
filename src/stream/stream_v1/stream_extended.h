#pragma once

#include "operators/operators.h"

#include <vector>
#include <functional>
#include <algorithm>
#include <iterator>
#include <type_traits>
#include <memory>

#include <typeinfo>

namespace lipaboy_lib {

namespace stream_v1_space {

using std::vector;
using std::pair;
using std::string;
using std::unique_ptr;
using std::shared_ptr;

using namespace operators_space;

//-----------------Stream Extended class----------------------//

template <class TOperator, class... Rest>
class Stream : public Stream<Rest...> {
public:
    using size_type = size_t;
    using OperatorType = TOperator;
    using SubType = Stream<Rest...>;
    using ConstSubType = const SubType;
    using iterator = typename SubType::outside_iterator;
    using SubPtr = SubType *;
    using ConstSubTypePtr = const SubType *;
    using ValueType = typename SubType::ValueType;
    using Range = typename SubType::Range;
    using ActionSignature = void(Stream*);
    using ActionType = std::function<ActionSignature>;

    template <class Functor>
    using ExtendedStreamType = Stream<Functor, OperatorType, Rest...>;

    using ResultValueType = typename TOperator::template RetType<typename SubType::ResultValueType>;

    template <typename, typename...> friend class Stream;

    //----------------------Constructors----------------------//
public:

    template <class StreamSuperType_, class TFunctor_>
    explicit
    Stream (TFunctor_&& functor, StreamSuperType_&& obj) noexcept
        : SubType(std::forward<StreamSuperType_>(obj)), operator_(std::forward<TFunctor_>(functor))
    {
#ifdef DEBUG_STREAM_WITH_NOISY
        if constexpr (std::is_rvalue_reference<StreamSuperType_&&>::value)
                cout << "   Stream is extended by move-constructor" << endl;
        else
                cout << "   Stream is extended by copy-constructor" << endl;
#endif
    }
public:
    Stream (Stream const & obj)
        : SubType(static_cast<ConstSubType&>(obj)),
        operator_(obj.operator_)
    {
#ifdef DEBUG_STREAM_WITH_NOISY
        cout << "   StreamEx copy-constructed" << endl;
#endif
    }
    Stream (Stream&& obj) noexcept
        : SubType(std::move(obj)),
        operator_(std::move(obj.operator_))
    {
#ifdef DEBUG_STREAM_WITH_NOISY
        cout << "   StreamEx move-constructed" << endl;
#endif
    }

	//----------------------Methods API-----------------------//

            //-----------------Tools-------------------//
protected:
    static constexpr bool isNoGetTypeBefore() {
        return (TOperator::metaInfo != GET && SubType::isNoGetTypeBefore());
    }
    static constexpr bool isGeneratorProducing() {
        return SubType::isGeneratorProducing();
    }

public:
	inline static constexpr bool isInfinite() {
		return isGeneratorProducing() && isNoGetTypeBefore();
	}
	template <class TStream_>
	inline static constexpr void assertOnInfinite() { 
		SubType::template assertOnInfinite<TStream_>(); 
	}

protected:
    // shortness
    SubPtr superThisPtr() { return static_cast<SubPtr>(this); }
    ConstSubTypePtr constSuperThisPtr() const { return static_cast<ConstSubTypePtr>(this); }
    auto superNextElem() -> typename SubType::ResultValueType { return superThisPtr()->nextElem(); }
    bool superHasNext() { return superThisPtr()->hasNext(); }
    auto superCurrentElem() -> typename SubType::ResultValueType {
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
        operator_.template init<SubType>(*superThisPtr());
    }

public:
    ResultValueType nextElem() {
#ifdef WIN32
        return std::move(operator_.nextElem<SubType>(*superThisPtr()));
#else
        return std::move(operator_.template nextElem<SuperType>(*superThisPtr()));
#endif
    }

    // TODO: must be test
	ResultValueType currentElem() {
		// #Crutch: it is strange crutch because Visual Studio can't call the template method 
		// without argument
		// that can help it to deduce the type of template method
		// Solve: Problem in the intersection 
        //        of names (currentElem() of Stream and currentElem() of operator_)
#ifdef WIN32
        return std::move(operator_.currentElem<SubType>(*superThisPtr()));
#else
        return std::move(operator_.template currentElem<SuperType>(*superThisPtr()));
#endif
    }

    bool hasNext() {
#ifdef WIN32
        return operator_.hasNext<SubType>(*superThisPtr());
#else
        return operator_.template hasNext<SuperType>(*superThisPtr());
#endif
    }

	void incrementSlider() {
#ifdef WIN32
        operator_.incrementSlider<SubType>(*superThisPtr());
#else
        operator_.template incrementSlider<SuperType>(*superThisPtr());
#endif
	}


    //------------------------------------------------------------------------//
    //-----------------------------Slider API Ends----------------------------//
    //------------------------------------------------------------------------//


protected:
    Range & range() { return superThisPtr()->range(); }
    const Range & range() const { return constSuperThisPtr()->range(); }

public:
    TOperator const & operation() const { return operator_; }

	// Not strong condition (because you don't compare the operations
    bool operator==(Stream & other) { return equals(other); }
    bool operator!=(Stream & other) { return !((*this) == other); }
private:
    bool equals(Stream & other) {
        return (operator_ == other.operator_
                && superThisPtr()->equals(static_cast<SubType&>(other))
                );
    }

    //---------------------------------------------------//
    //---------------------Fields------------------------//
    //---------------------------------------------------//

protected:
    TOperator operator_;

};

}

}
