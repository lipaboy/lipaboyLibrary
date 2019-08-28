#pragma once

#include "tools.h"
#include "reduce.h"

#include <functional>
#include <exception>
#include <memory>
#include <optional>

namespace lipaboy_lib {

	namespace stream {

		namespace operators {

			//---------------------------------------------------------------------------------------------------//
			//-----------------------------------Terminated operation-------------------------------------------//
			//---------------------------------------------------------------------------------------------------//

		

			namespace {
				template <class T>
				using TSelfReduce = reduce<std::function<T (T const &, T const &)> >;
			}

			template <class T>
			struct max : 
				public TSelfReduce<T>,
				TReturnSameType
			{
				static constexpr OperatorMetaTypeEnum metaInfo = MAX;
				static constexpr bool isTerminated = true;

				template <class T>
				using RetType = typename TSelfReduce<T>::template RetType<T>;

			public:
				max(T init)
					: TSelfReduce<T>( 
						[](T const & first, T const & second) -> T 
						{ 
							return first >= second ? first : second; 
						}) 
				{}

				template <class Stream_>
				auto apply(Stream_ & obj) -> typename TSelfReduce<T>::template RetType<T>
				{
					return TSelfReduce<T>::template apply<Stream_>(obj);
				}

			// i'am lipa boy (by Kirill Ponomarev)
			};

			/*using operators::max;
			using operators::max_impl;*/


		/*	template <class TStream>
			struct shortening::StreamTypeExtender<TStream, max<typename TStream::ResultValueType> > {
				template <class T>
				using remref = std::remove_reference_t<T>;

				using type = typename remref<TStream>::template ExtendedStreamType<
					remref<operators::max_impl<typename TStream::ResultValueType> > >;
			};*/

			//-------------------------------------------------------------//
			//--------------------------Apply API--------------------------//
			//-------------------------------------------------------------//

		}

	}

}

