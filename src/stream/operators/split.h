#pragma once

#include "tools.h"

#include <vector>
#include <string>
#include <type_traits>
#include <memory>

namespace lipaboy_lib {

	namespace stream {

		namespace operators {

			// TODO: replace SplitPredicate to regular expression checking

			using std::vector;
			using std::string;
			using std::shared_ptr;

			using std::cout;
			using std::endl;

			using lipaboy_lib::function_traits;


			// split operator get input parameter - SplitPredicate - predicate functor that say when you must skip elem
			// and finish grouping input elements into some container

			template <class TContainer>
			struct split 
				: public FunctorHolder<
					std::function<bool(typename TContainer::value_type)>
				>
			{
				static constexpr OperatorMetaTypeEnum metaInfo = SPLIT;
				static constexpr bool isTerminated = false;

				using ValueType = typename TContainer::value_type;
				using SplitPredicate = std::function<bool(ValueType)>;
				using ContainerType = TContainer;

			public:
				split(SplitPredicate splitFunctor)
					: FunctorHolder<SplitPredicate>(splitFunctor)
				{}
			};

			template <class SplitPredicate, class TContainer = std::string>
			struct split_impl : public FunctorHolder<SplitPredicate>
			{
			public:
				using size_type = size_t;

				using ValueType = char;
				template <class T>
				using RetType = TContainer;

				using ReturnType = RetType<ValueType>;
				using const_reference = const ReturnType &;

				static constexpr OperatorMetaTypeEnum metaInfo = SPLIT;
				static constexpr bool isTerminated = false;

			public:
				split_impl(SplitPredicate splitFunctor) 
					: FunctorHolder<SplitPredicate>(splitFunctor) 
				{}
				split_impl(split<TContainer> obj)
					: FunctorHolder<SplitPredicate>(obj.functor())
				{}

				template <class TSubStream>
				auto nextElem(TSubStream& stream) -> ReturnType
				{
					ReturnType part;

					for (size_type i = 0; stream.hasNext(); i++) {
						auto temp = stream.nextElem();
						if (FunctorHolder<SplitPredicate>::functor()(temp))
							break;
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
					return stream.hasNext();
				}

			private:
			};

		}

		using operators::split;
		using operators::split_impl;

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
				>
			>;
		};

	}

}

