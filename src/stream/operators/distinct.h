#pragma once

#include "tools.h"
#include "filter.h"

#include <memory>
#include <unordered_set>
#include <functional>

namespace lipaboy_lib {

	namespace stream {

		namespace operators {

			using std::shared_ptr;

			// TODO: test unordered_set<ref> with Noisy objects

			//-------------------------------------------------------------------------------------//
			//--------------------------------Unterminated operation------------------------------//
			//-------------------------------------------------------------------------------------//

			struct distinct : public TReturnSameType
			{
				static constexpr OperatorMetaTypeEnum metaInfo = DISTINCT;
				static constexpr bool isTerminated = false;
			};

			template <class T>
			struct distinct_impl : public filter_impl<std::function<bool(T&)>, T>
			{
				static constexpr OperatorMetaTypeEnum metaInfo = DISTINCT;
				static constexpr bool isTerminated = false;
				
				using type = T;
				using reference = std::reference_wrapper<type>;
				using ContainerType = std::unordered_set<reference, 
					std::hash<std::remove_const_t<type> >, std::equal_to<type> >;
				using ContainerTypePtr = shared_ptr<ContainerType>;
			public:
				distinct_impl(distinct obj)	
					// init by stopper, not more
					: filter_impl(std::function<bool(T&)>([](T& elem) { return true; }))
				{
					pDistinctSet_ = std::make_shared<ContainerType>();
					ContainerTypePtr lol = pDistinctSet_;
#ifdef LOL_DEBUG_NOISY
					std::cout << "\tset is created" << endl;
#endif
					// set by real functor of filtering
					this->setFunctor(std::function<bool(T&)>(
						[set = lol](T & elem) -> bool 
						{
#ifdef LOL_DEBUG_NOISY
							std::cout << "\t  start inserting " << endl;
#endif
							bool isInserted = set->insert(std::ref(elem)).second;
#ifdef LOL_DEBUG_NOISY
							std::cout << "\t  end inserting " << endl;
#endif
							if (isInserted) 
								return true;
							return false;
						}));
				}

#ifdef LOL_DEBUG_NOISY
				~distinct_impl() {
					std::cout << "\tdestruct distinct" << endl;
				}
#endif

			private:
				ContainerTypePtr pDistinctSet_;
			};

		}

		using operators::distinct;
		using operators::distinct_impl;

		template <class TStream>
		struct shortening::StreamTypeExtender<TStream, distinct> {
			template <class T>
			using remref = std::remove_reference_t<T>;

			using type = typename remref<TStream>::template ExtendedStreamType<
				remref<distinct_impl<typename TStream::ResultValueType> > >;
		};

	}

}