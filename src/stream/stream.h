#pragma once

#include "operators/operators.h"
#include "stream_extended.h"
#include "stream_base.h"
#include "extra_tools/initializer_list_iterator.h"
#include "extra_tools/producing_iterator.h"

namespace lipaboy_lib {

	namespace stream_space {

		//-------------Different types of Stream---------------//

		template <class TIterator>
		using StreamOfOutsideIterators = StreamBase<TIterator>;

		template <class T>
		using StreamOfInitializingList = StreamBase<InitializerListIterator<T>
			//typename std::initializer_list<T>::iterator
		>;

		template <class Generator>
		using StreamOfGenerator = StreamBase<ProducingIterator<typename std::result_of<Generator(void)>::type> >;


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

		template <class Generator>
		auto Stream(Generator&& generator)
			-> StreamOfGenerator<Generator>
		{
			return StreamOfGenerator<Generator>(std::forward<Generator>(generator));
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

		template <class TIterator>
		auto allocateStream(TIterator begin, TIterator end)
			-> StreamOfOutsideIterators<TIterator> *
		{
			return new StreamOfOutsideIterators<TIterator>(begin, end);
		}

		template <class T>
		auto allocateStream(std::initializer_list<T> init)
			-> StreamOfInitializingList<T> *
		{
			return new StreamOfInitializingList<T>(init);
		}

		template <class T, class... Args>
		auto allocateStream(T elem, Args... args)
			-> StreamOfInitializingList<T> *
		{
			return new StreamOfInitializingList<T>({ elem, args... });
		}

		template <class Generator>
		auto allocateStream(Generator&& generator)
			-> StreamOfGenerator<Generator> *
		{
			return new StreamOfGenerator<Generator>(std::forward<Generator>(generator));
		}

		template <class Container>
		auto Stream(Container const & container)
			-> StreamOfOutsideIterators<typename Container::const_iterator>
		{
			return StreamOfOutsideIterators<typename Container::const_iterator>(
				cbegin(container), cend(container));
		}

		

		//--------------------------------------------------------------------------//
		//------------------Extending stream by concating operations-----------------//
		//--------------------------------------------------------------------------//

		// You cannot union these functions into one with Forward semantics because it will be too generalized
		// Example (this function will apply for such expression: std::ios::in | std::ios::out)

		namespace shortening {

			template <class TOperator, class... Args>
			using ExtendingReturnType =
				lipaboy_lib::enable_if_else_t<TOperator::isTerminated,
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

			if constexpr (TOperator::isTerminated == true) {
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

			if constexpr (TOperator::isTerminated == true) {
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

	}

}
