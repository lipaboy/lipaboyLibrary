#pragma once

#include "operators/operators.h"
#include "stream_extended.h"
#include "stream_base.h"
#include "extra_tools/initializer_list_iterator.h"
#include "extra_tools/producing_iterator.h"

namespace lipaboy_lib::short_stream {

	//-------------Different types of Stream---------------//

	template <class TIterator>
	using StreamOfOutsideIterators = ShortStream<TIterator>;

	template <class T>
	using StreamOfInitializingList = ShortStream<InitializerListIterator<T>
		//typename std::initializer_list<T>::iterator
	>;

	template <class Generator>
	using StreamOfGenerator = ShortStream<
		lipaboy_lib::ProducingIterator<typename std::result_of<Generator(void)>::type> >;


	//-------------------Wrappers-----------------------//

	template <class TIterator>
	auto buildShortStream(TIterator begin, TIterator end)
		-> StreamOfOutsideIterators<TIterator>
	{
		return StreamOfOutsideIterators<TIterator>(begin, end);
	}

	template <class T>
	auto buildShortStream(std::initializer_list<T> init)
		-> StreamOfInitializingList<T>
	{
		return StreamOfInitializingList<T>(init);
	}

	template <class T, class... Args>
	auto buildShortStream(T elem, Args... args)
		-> StreamOfInitializingList<T>
	{
		return StreamOfInitializingList<T>({ elem, args... });
	}

	template <class Generator>
	auto buildShortStream(Generator&& generator)
		-> StreamOfGenerator<Generator>
	{
		return StreamOfGenerator<Generator>(std::forward<Generator>(generator));
	}

	template <class T, size_t size>
	auto buildShortStream(T(&init)[size])
		-> StreamOfOutsideIterators<std::move_iterator<T*> >
	{
		return StreamOfOutsideIterators<std::move_iterator<T*> >(
			std::make_move_iterator<T*>(std::begin(init)),
			std::make_move_iterator<T*>(std::end(init))
			);
	}

	template <class Container>
	auto buildShortStream(Container const& container)
		-> StreamOfOutsideIterators<typename Container::const_iterator>
	{
		return StreamOfOutsideIterators<typename Container::const_iterator>(
			container.cbegin(), container.cend());
	}

	//--------------------------------------------------------------------------//
	//------------------Extending stream by concating operations-----------------//
	//--------------------------------------------------------------------------//

	// You cannot union these functions into one with Forward semantics because it will be to common
	// Example (this function will apply for such expression: std::ios::in | std::ios::out)

	template <class TOperator, class... Args>
	auto operator| (ShortStream<Args...>& stream, TOperator operation)
		-> lipaboy_lib::enable_if_else_t<TOperator::isTerminated,
		typename TOperator::template RetType<typename ShortStream<Args...>::ResultValueType>,
		short_stream::shortening::StreamTypeExtender_t<ShortStream<Args...>, TOperator> >
	{
		using StreamType = ShortStream<Args...>;

		if constexpr (TOperator::isTerminated == true) {
			stream.template assertOnInfinite<StreamType>();
			return operation.apply(stream);
		}
		else {
			//static_assert(TOperator::isTerminated , "lol1");
			return shortening::StreamTypeExtender_t<StreamType, TOperator>
				(operation, stream);
		}
	}

	template <class TOperator, class... Args>
	auto operator| (ShortStream<Args...>&& stream, TOperator operation)
		-> lipaboy_lib::enable_if_else_t<TOperator::isTerminated,
		typename TOperator::template RetType<typename ShortStream<Args...>::ResultValueType>,
		short_stream::shortening::StreamTypeExtender_t<ShortStream<Args...>, TOperator> >
	{
		using StreamType = ShortStream<Args...>;

		if constexpr (TOperator::isTerminated == true) {
			stream.template assertOnInfinite<StreamType>();
			return operation.apply(stream);
		}
		else
			return shortening::StreamTypeExtender_t<StreamType, TOperator>
			(operation, std::move(stream));
	}

}

