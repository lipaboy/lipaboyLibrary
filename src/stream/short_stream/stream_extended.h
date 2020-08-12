#pragma once

#include "stream/operators/operators.h"

#include <functional>
#include <type_traits>

namespace lipaboy_lib::short_stream {

	using std::cout;
	using std::endl;

	using short_stream::operators::get;

	//-----------------Stream Extended class----------------------//

	template <class TOperator, class... Rest>
	class ShortStream : public ShortStream<Rest...> {
	public:
		using size_type = size_t;
		using OperatorType = TOperator;
		using SubType = ShortStream<Rest...>;
		using ConstSubType = const SubType;
		using iterator = typename SubType::outside_iterator;
		using SubTypePtr = SubType*;
		using ConstSubTypePtr = const SubType*;
		using ValueType = typename SubType::ValueType;

	public:
		template <class Functor>
		using ExtendedStreamType = ShortStream<Functor, OperatorType, Rest...>;

		using ResultValueType = typename TOperator::template RetType<
			typename SubType::ResultValueType>;

	public:
		template <typename, typename...> friend class ShortStream;

		//----------------------Constructors----------------------//
	public:

		template <class StreamSubType_, class TFunctor_>
		explicit
			ShortStream(TFunctor_&& functor, StreamSubType_&& obj) noexcept
			: SubType(std::forward<StreamSubType_>(obj)), operator_(std::forward<TFunctor_>(functor))
		{}
	public:
		ShortStream(ShortStream const& obj)
			: SubType(static_cast<ConstSubType&>(obj)),
			operator_(obj.operator_)
		{}
		ShortStream(ShortStream&& obj) noexcept
			: SubType(std::move(obj)),
			operator_(std::move(obj.operator_))
		{}

		//----------------------Methods API-----------------------//

		//-----------------Tools-------------------//
	protected:
		static constexpr bool isNoGetTypeBefore() {
			return (!std::is_same_v<get, TOperator>
				&& SubType::isNoGetTypeBefore());
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
		// shortnesses
		SubTypePtr subThisPtr() { return static_cast<SubTypePtr>(this); }
		ConstSubTypePtr constSuperThisPtr() const { return static_cast<ConstSubTypePtr>(this); }
		auto superNextElem() -> typename SubType::ResultValueType { return subThisPtr()->nextElem(); }
		bool superHasNext() { return subThisPtr()->hasNext(); }

		//------------------------------------------------------------------------//
		//-----------------------------Slider API---------------------------------//
		//------------------------------------------------------------------------//

	public:

		ResultValueType next() {
			return operator_.template next<SubType>(*subThisPtr());
		}
		void incrementSlider() {
			operator_.template incrementSlider<SubType>(*subThisPtr());
		}


		//------------------------------------------------------------------------//
		//-----------------------------Slider API Ends----------------------------//
		//------------------------------------------------------------------------//

		bool operator==(ShortStream& other) { return equals(other); }
		bool operator!=(ShortStream& other) { return !((*this) == other); }
	private:
		bool equals(ShortStream& other) {
			return (operator_ == other.operator_
				&& subThisPtr()->equals(static_cast<SubType&>(other))
				);
		}

	public:
		TOperator const& operation() const { return operator_; }

		//---------------------------------------------------//
		//---------------------Fields------------------------//
		//---------------------------------------------------//

	protected:
		TOperator operator_;

	};

}

