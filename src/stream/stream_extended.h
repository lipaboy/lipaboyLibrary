#pragma once

#include <functional>
#include <iterator>
#include <type_traits>

#ifdef DEBUG_STREAM_WITH_NOISY
#include <iostream>
#endif

namespace lipaboy_lib::stream_space {

	//-----------------Stream Extended class----------------------//

	template <class TOperator, class... Rest>
	class StreamBase : public StreamBase<Rest...> {
	public:
		using size_type = size_t;
		using OperatorType = TOperator;
		using SubType = StreamBase<Rest...>;
		using ConstSubType = const SubType;
        using outside_iterator = typename SubType::outside_iterator;
		using SubTypePtr = SubType * ;
		using ConstSubTypePtr = const SubType *;

	public:
		template <class Functor>
		using ExtendedStreamType = StreamBase<Functor, OperatorType, Rest...>;

		using ResultValueType = typename TOperator::template RetType<
			typename SubType::ResultValueType>;

	public:
		template <typename, typename...> friend class StreamBase;

		//----------------------Constructors----------------------//
	public:

		template <class StreamSubType_, class TFunctor_>
		explicit
			StreamBase(TFunctor_&& functor, StreamSubType_&& obj) noexcept
			: SubType(std::forward<StreamSubType_>(obj)), operator_(std::forward<TFunctor_>(functor))
		{
#ifdef DEBUG_STREAM_WITH_NOISY
			if constexpr (std::is_rvalue_reference<StreamSubType_&&>::value)
				std::cout << "   Stream is extended by move-constructor" << std::endl;
			else
				std::cout << "   Stream is extended by copy-constructor" << std::endl;
#endif
		}
	public:
		StreamBase(StreamBase const & obj)
			: SubType(static_cast<ConstSubType&>(obj)),
			operator_(obj.operator_)
		{
#ifdef DEBUG_STREAM_WITH_NOISY
			std::cout << "   StreamEx copy-constructed" << std::endl;
#endif
		}
		StreamBase(StreamBase&& obj) noexcept
			: SubType(std::move(obj)),
			operator_(std::move(obj.operator_))
		{
#ifdef DEBUG_STREAM_WITH_NOISY
			std::cout << "   StreamEx move-constructed" << std::endl;
#endif
		}

		//----------------------Methods API-----------------------//

		//-----------------Tools-------------------//
	protected:
		static constexpr bool isNoFixSizeOperatorBefore() {
            return (!std::is_base_of_v<op::impl::FixSizeOperator, TOperator>
				&& SubType::isNoFixSizeOperatorBefore());
		}
		static constexpr bool isGeneratorProducing() {
			return SubType::isGeneratorProducing();
		}

	public:
		inline static constexpr bool isInfinite() {
			return isGeneratorProducing() && isNoFixSizeOperatorBefore();
		}
		template <class TStream_>
		inline static constexpr void assertOnInfinite() {
			SubType::template assertOnInfinite<TStream_>();
		}

	protected:
		// shortnesses
		SubTypePtr subThisPtr() { return static_cast<SubTypePtr>(this); }
		ConstSubTypePtr constSubThisPtr() const { return static_cast<ConstSubTypePtr>(this); }
		auto subNextElem() -> typename SubType::ResultValueType { return subThisPtr()->nextElem(); }
		bool subHasNext() { return subThisPtr()->hasNext(); }

		//------------------------------------------------------------------------//
		//-----------------------------Slider API---------------------------------//
		//------------------------------------------------------------------------//

	public:

		ResultValueType nextElem() {
            return operator_.template nextElem<SubType>(*subThisPtr());
		}

		bool hasNext() {
            return operator_.template hasNext<SubType>(*subThisPtr());
		}

		void incrementSlider() {
            operator_.template incrementSlider<SubType>(*subThisPtr());
		}


		//------------------------------------------------------------------------//
		//-----------------------------Slider API Ends----------------------------//
		//------------------------------------------------------------------------//

		bool operator==(StreamBase & other) { return equals(other); }
		bool operator!=(StreamBase & other) { return !((*this) == other); }
	private:
		bool equals(StreamBase & other) {
			return (operator_ == other.operator_
				&& subThisPtr()->equals(static_cast<SubType&>(other))
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
