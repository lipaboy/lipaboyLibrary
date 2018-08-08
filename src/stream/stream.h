#pragma once

#include "stream/stream_extended.h"
#include "stream/stream_base.h"
#include "stream/range.h"
#include "stream/initializer_list_iterator.h"
#include "stream/producing_iterator.h"

namespace lipaboy_lib {

namespace stream_space {

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
	auto allocateStream(std::initializer_list<T>)
		-> StreamOfInitializingList<T> *
	{
		return new StreamOfInitializingList<T>(begin, end);
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

}

}
