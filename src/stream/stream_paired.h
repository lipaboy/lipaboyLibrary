#pragma once

//#include "operators/operators.h"
#include "operators/get.h"

#include <vector>
#include <functional>
#include <algorithm>
#include <iterator>
#include <type_traits>
#include <memory>

#include <typeinfo>

#include <iostream>

namespace lipaboy_lib::stream_space {

    using operators::get;

    //-----------------Stream Extended class----------------------//

    template <class TOperator, class FirstSuperStream, class SecondSuperStream>
    class StreamPairedBase :
            public FirstSuperStream,
            public SecondSuperStream
    {
    public:
        using OperatorType = TOperator;
        using FirstSubType = FirstSuperStream;
        using SecondSubType = SecondSuperStream;
        using FirstConstSubType = const FirstSubType;
        using SecondConstSubType = const SecondSubType;
        using first_iterator = typename FirstSubType::outside_iterator;
        using second_iterator = typename SecondSubType::outside_iterator;
        using FirstSubTypePtr = FirstSubType * ;
        using SecondSubTypePtr = SecondSubType * ;
        using FirstConstSubTypePtr = const FirstSubType *;
        using SecondConstSubTypePtr = const SecondSubType *;
        using FirstValueType = typename FirstSubType::ValueType;
        using SecondValueType = typename SecondSubType::ValueType;
//        using ActionSignature = void(StreamBase*);
//        using ActionType = std::function<ActionSignature>;

//    public:
//        template <class Functor>
//        using ExtendedStreamType = StreamBase<Functor, OperatorType, Rest...>;

        using ResultValueType = typename TOperator::template RetType<
            typename FirstSubType::ResultValueType, typename SecondSubType::ResultValueType>;

    public:
        //template <typename, typename...> friend class StreamBase;

        //----------------------Constructors----------------------//
    public:

        template <class FirstSubType_, class SecondSubType_, class TFunctor_>
        explicit
            StreamPairedBase(TFunctor_&& functor, FirstSubType_&& firstObj, SecondSubType_&& secondObj) noexcept
            : FirstSubType(std::forward<FirstSubType_>(firstObj)),
              SecondSubType(std::forward<SecondSubType_>(secondObj)),
              operator_(std::forward<TFunctor_>(functor))
        {
#ifdef LOL_DEBUG_NOISY
            if constexpr (std::is_rvalue_reference<StreamSubType_&&>::value)
                std::cout << "   Stream is extended by move-constructor" << std::endl;
            else
                std::cout << "   Stream is extended by copy-constructor" << std::endl;
#endif
        }
    public:
        StreamPairedBase(StreamPairedBase const & obj)
            : FirstSubType(static_cast<FirstConstSubType&>(obj)),
              SecondSubType(static_cast<SecondConstSubType&>(obj)),
              operator_(obj.operator_)
        {
#ifdef LOL_DEBUG_NOISY
            std::cout << "   StreamEx copy-constructed" << std::endl;
#endif
        }
        StreamPairedBase(StreamPairedBase&& obj) noexcept
            : FirstSubType(std::move(obj)),
              SecondSubType(std::move(obj)),
              operator_(std::move(obj.operator_))
        {
#ifdef LOL_DEBUG_NOISY
            std::cout << "   StreamEx move-constructed" << std::endl;
#endif
        }

        //----------------------Methods API-----------------------//

        //-----------------Tools-------------------//
    protected:
        static constexpr bool isNoGetTypeBefore() {
            return (!std::is_same_v<get, TOperator>
                && FirstSubType::isNoGetTypeBefore()
                && SecondSubType::isNoGetTypeBefore());
        }
        static constexpr bool isGeneratorProducing() {
            return FirstSubType::isGeneratorProducing()
                    || SecondSubType::isGeneratorProducing();
        }

    public:
        inline static constexpr bool isInfinite() {
            return isGeneratorProducing() && isNoGetTypeBefore();
        }
        template <class TStream_>
        inline static constexpr void assertOnInfinite() {
            FirstSubType::template assertOnInfinite<TStream_>();
        }

    protected:
        // shortnesses
        FirstSubTypePtr firstSuperThisPtr() { return static_cast<FirstSubTypePtr>(this); }
        SecondSubTypePtr secondSuperThisPtr() { return static_cast<SecondSubTypePtr>(this); }
        FirstConstSubTypePtr firstConstSuperThisPtr() const { return static_cast<FirstConstSubTypePtr>(this); }
        SecondConstSubTypePtr secondConstSuperThisPtr() const { return static_cast<SecondConstSubTypePtr>(this); }
        auto firstSuperNextElem() -> typename FirstSubType::ResultValueType { return firstSuperThisPtr()->nextElem(); }
        auto secondSuperNextElem() -> typename SecondSubType::ResultValueType { return secondSuperThisPtr()->nextElem(); }
        bool firstSuperHasNext() { return firstSuperThisPtr()->hasNext(); }
        bool secondSuperHasNext() { return secondSuperThisPtr()->hasNext(); }

        //------------------------------------------------------------------------//
        //-----------------------------Slider API---------------------------------//
        //------------------------------------------------------------------------//

    public:

        ResultValueType nextElem() {
            return operator_.template nextElem<FirstSubType, SecondSubType>(*firstSuperThisPtr(), *secondSuperThisPtr());
        }

        bool hasNext() {
            return operator_.template hasNext<FirstSubType, SecondSubType>(*firstSuperThisPtr(), *secondSuperThisPtr());
        }

        void incrementSlider() {
            operator_.template incrementSlider<FirstSubType, SecondSubType>(*firstSuperThisPtr(), *secondSuperThisPtr());
        }


        //------------------------------------------------------------------------//
        //-----------------------------Slider API Ends----------------------------//
        //------------------------------------------------------------------------//

        bool operator==(StreamPairedBase & other) { return equals(other); }
        bool operator!=(StreamPairedBase & other) { return !((*this) == other); }
    private:
        bool equals(StreamPairedBase & other) {
            return (operator_ == other.operator_
                && firstSuperThisPtr()->equals(static_cast<FirstSubType&>(other))
                && secondSuperThisPtr()->equals(static_cast<SecondSubType&>(other))
                );
        }

    public:
        TOperator const & operation() const { return operator_; }

        //---------------------------------------------------//
        //---------------------Fields------------------------//
        //---------------------------------------------------//

    protected:
        TOperator operator_;

    };

}

