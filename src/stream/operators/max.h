#pragma once

#include "tools.h"
#include "reduce.h"

#include <functional>
#include <exception>
#include <memory>
#include <optional>

namespace lipaboy_lib {

	namespace stream_space {

		namespace operators {

			//---------------------------------------------------------------------------------------------------//
			//-----------------------------------Terminated operation-------------------------------------------//
			//---------------------------------------------------------------------------------------------------//

			namespace {
				template <class T>
				using TSelfReduce = reduce_impl<std::function<T (T const &, T const &)> >;
			}

			struct max : public TReturnSameType
			{
				static constexpr OperatorMetaTypeEnum metaInfo = MAX;
				static constexpr bool isTerminated = true;

				max() {}
			};

			template <class T>
			struct max_impl : 
				public TSelfReduce<T>
			{
				static constexpr OperatorMetaTypeEnum metaInfo = MAX;
				static constexpr bool isTerminated = true;

                template <class TVal>
                using RetType = typename TSelfReduce<T>::template RetType<TVal>;

			public:
				max_impl(max) : 
					TSelfReduce<T>( 
						[](T const & first, T const & second) -> T 
						{ 
							return first >= second ? first : second; 
						}) 
				{}

				template <class Stream_>
				auto apply(Stream_ & obj) -> typename TSelfReduce<T>::template RetType<T>
				{
					static_assert(std::is_same_v<typename TSelfReduce<T>::template RetType<T>, std::optional<T> >,
						"Error69");
					return TSelfReduce<T>::template apply<Stream_>(obj);
				}

            // i'am lipa boy (by Kirill Ponomarev)

			};

            //-------------------------------------------------------------//
            //--------------------------Apply API--------------------------//
            //-------------------------------------------------------------//

        }

		using operators::max;
		using operators::max_impl;

		template <class TStream>
		struct shortening::TerminatedOperatorTypeApply<TStream, max> {
			using type = operators::max_impl<typename TStream::ResultValueType>;
		};

	}

}

