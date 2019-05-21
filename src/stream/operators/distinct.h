#pragma once

#include "tools.h"
#include "filter.h"

#include <memory>
#include <unordered_set>
#include <functional>

namespace lipaboy_lib {

	namespace stream_space {

		namespace operators_space {

			using std::shared_ptr;

			// TODO: test unordered_set<ref> with Noisy objects
			// BUG: don't work on Linux with gcc-7

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
                using ContainerType = std::unordered_set<
                    //reference,
                    type,
                    std::hash<std::remove_const_t<type> >,
                    std::equal_to<type> >;
				using ContainerTypePtr = shared_ptr<ContainerType>;

			public:
                distinct_impl(distinct)
                    : filter_impl<std::function<bool(T&)>, T>(std::function<bool(T&)>([](T&) { return true; }))
				{
					pDistinctSet_ = std::make_shared<ContainerType>();
					ContainerTypePtr lol = pDistinctSet_;
					this->setFunctor(std::function<bool(T&)>(
						[set = lol](T & elem) -> bool 
						{
                            bool isInserted = set->insert(elem).second;
                            //for (auto it = set->begin(); it != set->end(); it++) {
                            //    auto temp = *it;
                            //}
                            if (isInserted)
								return true;
							return false;
						}));
				}

			private:
				ContainerTypePtr pDistinctSet_;
			};

		}

		using operators_space::distinct;
		using operators_space::distinct_impl;

		template <class TStream>
		struct shortening::StreamTypeExtender<TStream, distinct> {
			template <class T>
			using remref = std::remove_reference_t<T>;

			using type = typename remref<TStream>::template ExtendedStreamType<
				remref<distinct_impl<typename TStream::ResultValueType> > >;
		};

	}

}
