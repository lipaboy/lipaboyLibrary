#pragma once

#include "stream_extended.h"
#include "stream_base.h"
#include "range.h"
#include "extra_tools/extra_tools.h"

namespace stream_space {

using lipaboy_lib::ProducingIterator;
using lipaboy_lib::InitializerListIterator;

template <class TIterator>
using StreamOfOutsideIterators = Stream<functors_space::IsOutsideIteratorsRefer, TIterator>;

template <class T>
using StreamOfInitializingList = 
	Stream<IsInitializingListCreation, InitializerListIterator<T> >;

template <class Generator>
using StreamOfGenerator = Stream<IsGeneratorProducing, 
								 ProducingIterator<typename std::result_of<Generator(void)>::type>
								/*typename OwnContainerTypeWithoutValueType<
									typename std::result_of<Generator(void)>::type
								>::iterator*/
								>;

template <class T>
using StreamOfGenerator_t = Stream<IsGeneratorProducing,
	typename OwnContainerTypeWithoutValueType<T>::iterator
>;


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
    return StreamOfInitializingList<T>({elem, args...});
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
