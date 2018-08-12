#pragma once

#include "stream_extended.h"
#include "stream_basic.h"
#include "range.h"
#include "initializer_list_iterator.h"
#include "producing_iterator.h"

namespace lipaboy_lib {

namespace stream_space {

	

	//-------------Different types of Stream---------------//

	template <class TIterator>
	using StreamOfOutsideIterators = Stream<TIterator>;

	template <class T>
	using StreamOfInitializingList = Stream<InitializerListIterator<T> >;

	template <class Generator>
	using StreamOfGenerator = Stream<ProducingIterator<typename std::result_of<Generator(void)>::type> >;


	//-------------------Wrappers-----------------------//

	template <class TIterator>
	auto buildStream(TIterator begin, TIterator end)
		-> StreamOfOutsideIterators<TIterator>
	{
		return StreamOfOutsideIterators<TIterator>(begin, end);
	}

	template <class TIterator>
	auto allocateStream(TIterator begin, TIterator end)
		-> StreamOfOutsideIterators<TIterator> *
	{
		return new StreamOfOutsideIterators<TIterator>(begin, end);
	}

    template <class T>
    auto buildStream(std::initializer_list<T> init)
        -> StreamOfInitializingList<T>
    {
        return StreamOfInitializingList<T>(init);
    }

	template <class T>
    auto allocateStream(std::initializer_list<T> init)
		-> StreamOfInitializingList<T> *
	{
        return new StreamOfInitializingList<T>(init);
	}

    template <class T, class... Args>
    auto buildStream(T elem, Args... args)
        -> StreamOfInitializingList<T>
    {
        return StreamOfInitializingList<T>({ elem, args... });
    }

	template <class T, class... Args>
	auto allocateStream(T elem, Args... args)
		-> StreamOfInitializingList<T> *
	{
		return new StreamOfInitializingList<T>({ elem, args... });
	}

    template <class Generator>
    auto buildStream(Generator&& generator)
        -> StreamOfGenerator<Generator>
    {
        return StreamOfGenerator<Generator>(std::forward<Generator>(generator));
    }

	template <class Generator>
	auto allocateStream(Generator&& generator)
		-> StreamOfGenerator<Generator> *
	{
		return new StreamOfGenerator<Generator>(std::forward<Generator>(generator));
	}

	//--------------------------------------------------------------------------//
	//------------------Extending stream by concating operations-----------------//
	//--------------------------------------------------------------------------//

	// You cannot union these functions into one with Forward semantics because it will be to common
	// Example (this function will apply for such expression: std::ios::in | std::ios::out)

	template <class TOperation, class... Args>
	auto operator| (Stream<Args...>& stream, TOperation operation)
		-> lipaboy_lib::enable_if_else_t<TOperation::isTerminated, 
				typename TOperation::template RetType<typename Stream<Args...>::ResultValueType>,
				shortening::StreamTypeExtender_t<Stream<Args...>, TOperation> >
	{
		using StreamType = Stream<Args...>;

		if constexpr (TOperation::isTerminated == true) {
				stream.template assertOnInfiniteStream<StreamType>();
				return operation.apply(stream);
		}
		else
				return shortening::StreamTypeExtender_t<StreamType, TOperation>
					(operation, stream);
	}

	template <class TOperation, class... Args>
	auto operator| (Stream<Args...>&& stream, TOperation operation)
		-> lipaboy_lib::enable_if_else_t<TOperation::isTerminated,
				typename TOperation::template RetType<typename Stream<Args...>::ResultValueType>,
				shortening::StreamTypeExtender_t<Stream<Args...>, TOperation> >
	{
		using StreamType = Stream<Args...>;

		if constexpr (TOperation::isTerminated == true) {
			stream.template assertOnInfiniteStream<StreamType>();
			return operation.apply(stream);
		}
		else
			return shortening::StreamTypeExtender_t<StreamType, TOperation>
			(operation, std::move(stream));
	}
}

}
