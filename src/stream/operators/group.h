#pragma once

#include "tools.h"

#include <vector>
#include <string>
#include <type_traits>
#include <memory>

namespace lipaboy_lib {

	namespace stream {

		namespace operators {

			using std::vector;
			using std::string;
			using std::shared_ptr;

			using std::cout;
			using std::endl;

			using lipaboy_lib::function_traits;

			//struct group {
			//public:
			//	using size_type = size_t;

			//	template <class T>
			//	using RetType = vector<T>;

			//	//static constexpr OperatorMetaTypeEnum metaInfo = GROUP_BY_VECTOR;
			//	static constexpr bool isTerminated = false;
			//public:
			//	group(size_type partSize) : partSize_(partSize) {
			//		if (partSize == 0)
			//			throw std::logic_error("Parameter of GroupType constructor must be positive");
			//	}

			//	size_type part() const { return partSize_; }

			//private:
			//	size_type partSize_;
			//};


			// group operator get input parameter - SplitPredicate - predicate functor that say when you must skip elem
			// and finish grouping input elements into some container

			template <class SplitPredicate>
			struct group : public FunctorHolder<SplitPredicate>
			{
			public:
				using size_type = size_t;

				using ValueType = char;
				template <class T>
				using RetType = std::string;

				using ReturnType = RetType<ValueType>;
				using const_reference = const ReturnType &;

				static constexpr OperatorMetaTypeEnum metaInfo = GROUP;
				static constexpr bool isTerminated = false;
			public:
				group(SplitPredicate splitFunctor) 
					: FunctorHolder<SplitPredicate>(splitFunctor) 
				{}

				template <class TSubStream>
				auto nextElem(TSubStream& stream) -> ReturnType
				{
					ReturnType part;

					for (size_type i = 0; stream.hasNext(); i++) {
						auto temp = stream.nextElem();
						if (FunctorHolder<SplitPredicate>::functor()(temp))
							break;
						part += temp;
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

		/*using operators::group;
		using operators::group_impl;

		template <class TStream>
		struct shortening::StreamTypeExtender<TStream, group_impl> {
			template <class T>
			using remref = std::remove_reference_t<T>;

			using type = typename remref<TStream>::template ExtendedStreamType<
				remref<group_by_vector_impl<typename TStream::ResultValueType> > >;
		};*/

	}

}

