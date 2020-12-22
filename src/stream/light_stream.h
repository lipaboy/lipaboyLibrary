#pragma once

#include "operators/tools.h"
#include "stream_extended.h"
#include "stream_base.h"

#include "extra_tools/initializer_list_iterator.h"
#include "extra_tools/producing_iterator.h"
#include "extra_tools/sequence_producing_iterator.h"

#include <type_traits>

namespace lipaboy_lib::stream_space {

	// HINT: light_stream.h - because without operators

	// What's stream at all?
	// Answer 1: Stream is not iterator (because can't return current element). 
	//		   Single-pass. Save sequence of elements (periphrase).
	// Answer 2: Lazy - {1, 2, 3} | map(square) | sum() -> transform to: (1^2 + 2^2 + 3^2)

	// PLAN FOR STREAM:
	//-----------------
	// TODO: Think about allocators (in Range when happen copying and creating own container)
	//       (maybe too partical case?)
	// TODO: test different lambdas (with const&T in return type, with T& in argument type)
	// TODO: make Noisy test for reduce operation
	// TODO: think about writing iterators for Stream 
	//			- Iterator must be storage temporary current values.
	//			- Interesting fact: all the std::algorithms are terminated operations.
	// TODO: write advises for them who will write operators for Stream, for example:
	//			- When you write operator and you have inner fields that must be initialized
	//			  by Stream's elem then you should do it into hasNext<>() because
	//			  likely it is first that called by all the terminated operations (or clients).
	//			  And furthermore you likely will call hasNext<>() into nextElem<>() and incrementSlider<>().
	//			  Or you can write private method init<>().
	//			- Note: if client call the hasNext<>() then he start using Stream. 
	//					It means that you can use all the API functions of Stream interface.



	//-------------Different types of Stream---------------//

	template <class TIterator>
	using StreamOfOutsideIterators = StreamBase<TIterator>;

	template <class T>
	using StreamOfInitializingList = StreamBase<InitializerListIterator<T>
		//typename std::initializer_list<T>::iterator
	>;

	template <class Generator>
	using StreamOfGenerator = StreamBase<ProducingIterator<typename std::result_of<Generator(void)>::type> >;

	template <class ValueType>
	using StreamOfSequenceGenerator = StreamBase<SequenceProducingIterator<ValueType> >;


	//-------------------Wrappers-----------------------//

	template <class TIterator>
	auto Stream(TIterator begin, TIterator end)
		-> StreamOfOutsideIterators<TIterator>
	{
		return StreamOfOutsideIterators<TIterator>(begin, end);
	}

	template <class T>
	auto Stream(std::initializer_list<T> init)
		-> StreamOfInitializingList<T>
	{
		return StreamOfInitializingList<T>(init);
	}

	template <class T, class... Args>
	auto Stream(T elem, Args... args)
		-> StreamOfInitializingList<T>
	{
		return StreamOfInitializingList<T>({ elem, args... });
	}

	template <class Generator>
	auto Stream(Generator&& generator)
		-> StreamOfGenerator<Generator>
	{
		return StreamOfGenerator<Generator>(std::forward<Generator>(generator));
	}

	template <class ValueType, class Generator>
	auto Stream(ValueType initValue, Generator generator)
		-> StreamOfSequenceGenerator<ValueType>
	{
		return StreamOfSequenceGenerator<ValueType>(initValue, generator);
	}

	template <class T, size_t size>
	auto Stream(T(&init)[size])
		-> StreamOfOutsideIterators<std::move_iterator<T*> >
	{
		return StreamOfOutsideIterators<std::move_iterator<T*> >(
			std::make_move_iterator<T*>(std::begin(init)),
			std::make_move_iterator<T*>(std::end(init))
			);
	}

	//----------Allocating----------//

	template <class TIterator>
	auto allocateStream(TIterator begin, TIterator end)
		-> StreamOfOutsideIterators<TIterator>*
	{
		return new StreamOfOutsideIterators<TIterator>(begin, end);
	}

	template <class T>
	auto allocateStream(std::initializer_list<T> init)
		-> StreamOfInitializingList<T>*
	{
		return new StreamOfInitializingList<T>(init);
	}

	template <class T, class... Args>
	auto allocateStream(T elem, Args... args)
		-> StreamOfInitializingList<T>*
	{
		return new StreamOfInitializingList<T>({ elem, args... });
	}

	template <class Generator>
	auto allocateStream(Generator&& generator)
		-> StreamOfGenerator<Generator>*
	{
		return new StreamOfGenerator<Generator>(std::forward<Generator>(generator));
	}

	template <class Container>
	auto Stream(Container const& container)
		-> StreamOfOutsideIterators<typename Container::const_iterator>
	{
		return StreamOfOutsideIterators<typename Container::const_iterator>(
			cbegin(container), cend(container));
	}

	// Note: such approach doesn't match for second constructor. 
	//		 For more information try to explore

	//template <class T>
	//auto Stream(std::initializer_list<T> init)
	//	-> StreamOfOutsideIterators<typename std::initializer_list<T>::iterator>
	//{
	//	return StreamOfOutsideIterators<typename std::initializer_list<T>::iterator>(
	//		begin(init), end(init));
	//}

	//template <class T, class... Args>
	//auto Stream(T elem, Args... args)
	//	-> StreamOfOutsideIterators<typename std::initializer_list<T>::iterator>
	//{
	//	return Stream({ elem, args... });
	//}


	//--------------------------------------------------------------------------//
	//------------------Extending stream by concating operations-----------------//
	//--------------------------------------------------------------------------//

	// You cannot union these functions into one with Forward semantics because it will be too generalized
	// Example (this function will apply for such expression: std::ios::in | std::ios::out)

	namespace shortening {

		template <class TOperator, class... Args>
		using ExtendingReturnType =
			lipaboy_lib::enable_if_else_t<std::is_base_of_v<operators::TerminatedOperator, TOperator>,
			// terminated
			typename shortening::TerminatedOperatorTypeApply_t<StreamBase<Args...>, TOperator>
			::template RetType<typename StreamBase<Args...>::ResultValueType>,
			// non-terminated
			shortening::StreamTypeExtender_t<StreamBase<Args...>, TOperator>
			>;

	}

	template <class TOperator, class... Args>
	auto operator| (StreamBase<Args...>& stream, TOperator operation)
		-> shortening::ExtendingReturnType<TOperator, Args...>
	{
		using StreamType = StreamBase<Args...>;

		if constexpr (std::is_base_of_v<operators::TerminatedOperator, TOperator>) {
			stream.template assertOnInfinite<StreamType>();
			return shortening::TerminatedOperatorTypeApply_t<StreamType, TOperator>
				(operation).apply(stream);
		}
		else {
			return shortening::StreamTypeExtender_t<StreamType, TOperator>
				(operation, stream);
		}
	}

	template <class TOperator, class... Args>
	auto operator| (StreamBase<Args...>&& stream, TOperator operation)
		-> shortening::ExtendingReturnType<TOperator, Args...>
	{
		using StreamType = StreamBase<Args...>;

		if constexpr (std::is_base_of_v<operators::TerminatedOperator, TOperator>) {
			stream.template assertOnInfinite<StreamType>();
			// INFO: needn't to move the stream because it is terminated operation
			//       and method 'apply' get the l-value stream.
			return shortening::TerminatedOperatorTypeApply_t<StreamType, TOperator>
				(operation).apply(stream);
		}
		else {
			return shortening::StreamTypeExtender_t<StreamType, TOperator>
				(operation, std::move(stream));
		}
	}

	//-------------------Paired Stream---------------------//

	template <class ...ArgsFirst, class ...ArgsSecond>
	auto operator&(StreamBase<ArgsFirst...>& first, StreamBase<ArgsSecond...>& second)
		-> shortening::StreamTypeExtender_t<StreamBase<ArgsFirst...>,
		operators::paired< StreamBase<ArgsSecond...> > >
	{
		return (first | operators::paired< StreamBase<ArgsSecond...> >(second));
	}

	template <class ...ArgsFirst, class ...ArgsSecond>
	auto to_pair(StreamBase<ArgsFirst...>& first, StreamBase<ArgsSecond...>& second)
		-> shortening::StreamTypeExtender_t<StreamBase<ArgsFirst...>,
		operators::paired< StreamBase<ArgsSecond...> > >
	{
		return first & second;
	}

}
