#pragma once

#include "stream_extended.h"
#include "stream_base.h"
#include "range.h"

namespace stream_space {

//-------------------Wrappers-----------------------//

template <class TIterator>
auto createStream(TIterator begin, TIterator end)
    -> Stream<IsOutsideIteratorsRefer, TIterator>
{
    return Stream<IsOutsideIteratorsRefer, TIterator>(begin, end);
}

template <class TIterator>
auto makeStream(TIterator begin, TIterator end)
    -> Stream<IsOutsideIteratorsRefer, TIterator> *
{
    return new Stream<IsOutsideIteratorsRefer, TIterator>(begin, end);
}

template <class T>
decltype(auto) createStream(std::initializer_list<T> init)
{
    return Stream<IsInitializingListCreation, typename OwnContainerTypeWithoutValueType<T>::iterator>(init);
}

template <class T, class... Args>
decltype(auto) createStream(T elem, Args... args)
{
    return Stream<IsInitializingListCreation,
            typename OwnContainerTypeWithoutValueType<T>::iterator>({elem, args...});
}

template <class Generator>
decltype(auto) createStream(Generator&& generator)
{
    return Stream<IsGeneratorProducing,
            typename OwnContainerTypeWithoutValueType<
                typename std::result_of<Generator(void)>::type>::iterator>(std::forward<Generator>(generator));
}

}
