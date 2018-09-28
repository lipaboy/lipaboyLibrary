#pragma once

#include "operators/operators.h"

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

		using namespace operators_space;

		using std::cout;
		using std::endl;

		//-----------------Stream Extended class----------------------//

		template <class TOperator, class... Rest>
		class Stream : public Stream<Rest...> {
		public:
			using size_type = size_t;
			using OperatorType = TOperator;
			using SuperType = Stream<Rest...>;
			using ConstSuperType = const SuperType;
			using iterator = typename SuperType::outside_iterator;
			using SuperTypePtr = SuperType * ;
			using ConstSuperTypePtr = const SuperType *;
			using ValueType = typename SuperType::ValueType;
			using Range = typename SuperType::Range;
			using ActionSignature = void(Stream*);
			using ActionType = std::function<ActionSignature>;

			template <class Functor>
			using ExtendedStreamType = Stream<Functor, OperatorType, Rest...>;

			using ResultValueType = typename TOperator::template RetType<typename SuperType::ResultValueType>;

			template <typename, typename...> friend class Stream;

			//----------------------Constructors----------------------//
		public:

			template <class StreamSuperType_, class TFunctor_>
			explicit
				Stream(TFunctor_&& functor, StreamSuperType_&& obj) noexcept
				: SuperType(std::forward<StreamSuperType_>(obj)), operator_(std::forward<TFunctor_>(functor))
			{
#ifdef LOL_DEBUG_NOISY
				if constexpr (std::is_rvalue_reference<StreamSuperType_&&>::value)
					cout << "   Stream is extended by move-constructor" << endl;
				else
					cout << "   Stream is extended by copy-constructor" << endl;
#endif
			}
		public:
			Stream(Stream const & obj)
				: SuperType(static_cast<ConstSuperType&>(obj)),
				operator_(obj.operator_)
			{
#ifdef LOL_DEBUG_NOISY
				cout << "   StreamEx copy-constructed" << endl;
#endif
			}
			Stream(Stream&& obj) noexcept
				: SuperType(std::move(obj)),
				operator_(std::move(obj.operator_))
			{
#ifdef LOL_DEBUG_NOISY
				cout << "   StreamEx move-constructed" << endl;
#endif
			}

			//----------------------Methods API-----------------------//

			//-----------------Tools-------------------//
		protected:
			static constexpr bool isNoGetTypeBefore() {
				return (TOperator::metaInfo != GET && SuperType::isNoGetTypeBefore());
			}
			static constexpr bool isGeneratorProducing() {
				return SuperType::isGeneratorProducing();
			}

		public:
			inline static constexpr bool isInfinite() {
				return isGeneratorProducing() && isNoGetTypeBefore();
			}
			template <class TStream_>
			inline static constexpr void assertOnInfinite() {
				SuperType::template assertOnInfinite<TStream_>();
			}

		protected:
			// shortnesses
			SuperTypePtr superThisPtr() { return static_cast<SuperTypePtr>(this); }
			ConstSuperTypePtr constSuperThisPtr() const { return static_cast<ConstSuperTypePtr>(this); }
			auto superNextElem() -> typename SuperType::ResultValueType { return superThisPtr()->nextElem(); }
			bool superHasNext() { return superThisPtr()->hasNext(); }

			//------------------------------------------------------------------------//
			//-----------------------------Slider API---------------------------------//
			//------------------------------------------------------------------------//

		public:

			ResultValueType nextElem() {
#ifdef WIN32
				return std::move(operator_.nextElem<SuperType>(*superThisPtr()));
#else
				return std::move(operator_.template nextElem<SuperType>(*superThisPtr()));
#endif
			}

			bool hasNext() {
#ifdef WIN32
				return operator_.hasNext<SuperType>(*superThisPtr());
#else
				return operator_.template hasNext<SuperType>(*superThisPtr());
#endif
			}

			void incrementSlider() {
#ifdef WIN32
				operator_.incrementSlider<SuperType>(*superThisPtr());
#else
				operator_.template incrementSlider<SuperType>(*superThisPtr());
#endif
			}


			//------------------------------------------------------------------------//
			//-----------------------------Slider API Ends----------------------------//
			//------------------------------------------------------------------------//

			bool operator==(Stream & other) { return equals(other); }
			bool operator!=(Stream & other) { return !((*this) == other); }
		private:
			bool equals(Stream & other) {
				return (operator_ == other.operator_
					&& superThisPtr()->equals(static_cast<SuperType&>(other))
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

}
