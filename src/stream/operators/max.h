#pragma once

#include "tools.h"
#include "reduce.h"

#include <functional>
#include <exception>
#include <memory>

namespace lipaboy_lib {

	namespace stream {

		namespace operators {

			//---------------------------------------------------------------------------------------------------//
			//-----------------------------------Terminated operation-------------------------------------------//
			//---------------------------------------------------------------------------------------------------//

			template <class T>
			struct max
			{
				static constexpr OperatorMetaTypeEnum metaInfo = MAX;
				static constexpr bool isTerminated = true;

				max(T init) : init_(init) {}

				T init_;
			};

			namespace {
				template <class T>
				using TSelfReduce = reduce<std::function<T (T const &, T const &)> >;
			}

			template <class T>
			struct max_impl : 
				public TSelfReduce<T>,
				TReturnSameType
			{
				static constexpr OperatorMetaTypeEnum metaInfo = MAX;
				static constexpr bool isTerminated = true;

				template <class T>
				using RetType = typename TSelfReduce<T>::template RetType<T>;

				max_impl(max<T> obj) 
					: TSelfReduce<T>( 
						[](T const & first, T const & second) -> T 
						{ 
							return first >= second ? first : second; 
						}, obj.init_) 
				{}

				template <class Stream_>
				auto apply(Stream_ & obj) -> typename Stream_::ResultValueType
				{
					return TSelfReduce<T>::template apply<Stream_>(obj);
				}

			};

			using operators::max;
			using operators::max_impl;

			// i'am lipa boy (by Kirill Ponomarev)

			template <class TStream>
			struct shortening::StreamTypeExtender<TStream, max<typename TStream::ResultValueType> > {
				template <class T>
				using remref = std::remove_reference_t<T>;

				using type = typename remref<TStream>::template ExtendedStreamType<
					remref<operators::max_impl<typename TStream::ResultValueType> > >;
			};

			//-------------------------------------------------------------//
			//--------------------------Apply API--------------------------//
			//-------------------------------------------------------------//

		}

	}

}

