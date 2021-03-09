#pragma once

#include "tools.h"

#include <vector>
#include <string>
#include <type_traits>
#include <memory>

namespace lipaboy_lib::stream_space {

    namespace op {

		// TODO: replace SplitPredicate to regular expression checking
		// TODO: make split_impl more common by working with different ValueType,
		//		not only with char.

		// INFO:
		// split operator get input parameter - SplitPredicate - predicate functor that say when you must 
		// finish grouping input elements into some container and start the new group.

		template <class TContainer>
		struct split 
			: public impl::FunctorHolder<
				std::function<bool(typename TContainer::value_type)>
			>
		{
			using ValueType = typename TContainer::value_type;
			using SplitPredicate = std::function<bool(ValueType)>;
			using ContainerType = TContainer;

			template <class T>
			using RetType = ContainerType;

		public:
			explicit
				split(SplitPredicate splitFunctor)
                    : impl::FunctorHolder<SplitPredicate>(splitFunctor)
				{}
		};

		namespace impl {

			using std::vector;
			using std::string;
			using std::shared_ptr;
			using lipaboy_lib::function_traits;

			template <class SplitPredicate, class TContainer = std::string>
			struct split_impl : public FunctorHolder<SplitPredicate>
			{
			public:
				using size_type = size_t;

				using ValueType = typename TContainer::value_type;
				template <class T>
				using RetType = TContainer;

				using ReturnType = RetType<ValueType>;
				using const_reference = const ReturnType&;

			public:
				explicit
					split_impl(SplitPredicate splitFunctor)
					: FunctorHolder<SplitPredicate>(splitFunctor)
				{}
				explicit
					split_impl(split<TContainer> obj)
					: FunctorHolder<SplitPredicate>(obj.functor())
				{}

				template <class TSubStream>
				auto nextElem(TSubStream& stream) -> ReturnType
				{
					ReturnType part;

					for (size_type i = 0; stream.hasNext(); i++) {
						auto temp = stream.nextElem();
						if (FunctorHolder<SplitPredicate>::functor()(temp)) {
							if (part.empty())
								continue;
							break;
						}
						part.push_back(temp);
					}

					return std::move(part);
				}

				template <class TSubStream>
				void incrementSlider(TSubStream& stream) {
					for (size_type i = 0; stream.hasNext(); i++) {
						if (FunctorHolder<SplitPredicate>::functor()(stream.nextElem()))
							break;
					}
				}

				template <class TSubStream>
				bool hasNext(TSubStream& stream) {
					// INFO: we cannot check current element with SplitPredicate
					//		because we must call the nextElem that change the stream.
					return stream.hasNext();
				}

			private:
			};

		}

	}

    using op::split;
    using op::impl::split_impl;

	template <class TStream, class TContainer>
	struct shortening::StreamTypeExtender<TStream, 
		split<TContainer> >
	{
		template <class T>
		using remref = std::remove_reference_t<T>;

		using type = typename remref<TStream>::template ExtendedStreamType<
			remref<
				split_impl<typename split<TContainer>::SplitPredicate,
					typename split<TContainer>::ContainerType
				>
				//split_impl<std::function<bool(char)>, std::string>
			>
		>;
	};

}

