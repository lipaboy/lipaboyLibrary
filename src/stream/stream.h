#pragma once

#include "operators/operators.h"
#include "stream_extended.h"
#include "stream_basic.h"
#include "extra_tools/initializer_list_iterator.h"
#include "extra_tools/producing_iterator.h"

namespace lipaboy_lib {

	namespace stream {

		//-------------Different types of Stream---------------//

		template <class TIterator>
		using StreamOfOutsideIterators = Stream<TIterator>;

		template <class T>
		using StreamOfInitializingList = Stream<InitializerListIterator<T>
			//typename std::initializer_list<T>::iterator
		>;

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


		template <class T, size_t size>
		auto buildStream(T(&init)[size])
			-> StreamOfOutsideIterators<std::move_iterator<T*> >
		{
			return StreamOfOutsideIterators<std::move_iterator<T*> >(
				std::make_move_iterator<T*>(std::begin(init)),
				std::make_move_iterator<T*>(std::end(init))
				);
		}

		template <class Container>
		auto buildStream(Container const & container)
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
		auto operator| (Stream<Args...>& stream, TOperator operation)
			-> lipaboy_lib::enable_if_else_t<TOperator::isTerminated,
			typename TOperator::template RetType<typename Stream<Args...>::ResultValueType>,
			shortening::StreamTypeExtender_t<Stream<Args...>, TOperator> >
		{
			using StreamType = Stream<Args...>;

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
		auto operator| (Stream<Args...>&& stream, TOperator operation)
			-> lipaboy_lib::enable_if_else_t<TOperator::isTerminated,
			typename TOperator::template RetType<typename Stream<Args...>::ResultValueType>,
			shortening::StreamTypeExtender_t<Stream<Args...>, TOperator> >
		{
			using StreamType = Stream<Args...>;

			if constexpr (TOperator::isTerminated == true) {
				stream.template assertOnInfinite<StreamType>();
				return operation.apply(stream);
			}
			else
				return shortening::StreamTypeExtender_t<StreamType, TOperator>
				(operation, std::move(stream));
		}

	}

	namespace fast_stream {

		//-------------Different types of Stream---------------//

		template <class TIterator>
		using StreamOfOutsideIterators = Stream<TIterator>;

		template <class T>
		using StreamOfInitializingList = Stream<InitializerListIterator<T>
			//typename std::initializer_list<T>::iterator
		>;

		template <class Generator>
		using StreamOfGenerator = Stream<
			lipaboy_lib::ProducingIterator<typename std::result_of<Generator(void)>::type> >;


		//-------------------Wrappers-----------------------//

		template <class TIterator>
		auto buildStream(TIterator begin, TIterator end)
			-> StreamOfOutsideIterators<TIterator>
		{
			return StreamOfOutsideIterators<TIterator>(begin, end);
		}

		template <class T>
		auto buildStream(std::initializer_list<T> init)
			-> StreamOfInitializingList<T>
		{
			return StreamOfInitializingList<T>(init);
		}

		template <class T, class... Args>
		auto buildStream(T elem, Args... args)
			-> StreamOfInitializingList<T>
		{
			return StreamOfInitializingList<T>({ elem, args... });
		}

		template <class Generator>
		auto buildStream(Generator&& generator)
			-> StreamOfGenerator<Generator>
		{
			return StreamOfGenerator<Generator>(std::forward<Generator>(generator));
		}

		template <class T, size_t size>
		auto buildStream(T(&init)[size])
			-> StreamOfOutsideIterators<std::move_iterator<T*> >
		{
			return StreamOfOutsideIterators<std::move_iterator<T*> >(
				std::make_move_iterator<T*>(std::begin(init)),
				std::make_move_iterator<T*>(std::end(init))
				);
		}

		template <class Container>
		auto buildStream(Container const & container)
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
		auto operator| (Stream<Args...>& stream, TOperator operation)
			-> lipaboy_lib::enable_if_else_t<TOperator::isTerminated,
			typename TOperator::template RetType<typename Stream<Args...>::ResultValueType>,
			shortening::StreamTypeExtender_t<Stream<Args...>, TOperator> >
		{
			using StreamType = Stream<Args...>;

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
		auto operator| (Stream<Args...>&& stream, TOperator operation)
			-> lipaboy_lib::enable_if_else_t<TOperator::isTerminated,
			typename TOperator::template RetType<typename Stream<Args...>::ResultValueType>,
			shortening::StreamTypeExtender_t<Stream<Args...>, TOperator> >
		{
			using StreamType = Stream<Args...>;

			if constexpr (TOperator::isTerminated == true) {
				stream.template assertOnInfinite<StreamType>();
				return operation.apply(stream);
			}
			else
				return shortening::StreamTypeExtender_t<StreamType, TOperator>
				(operation, std::move(stream));
		}

	}

	

}
