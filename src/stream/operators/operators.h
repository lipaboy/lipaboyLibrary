#pragma once

#include "tools.h"

// non-terminated operations
#include "filter.h"
#include "group_by_vector.h"
#include "get.h"
#include "skip.h"
#include "ungroup_by_bit.h"
#include "map.h"
#include "distinct.h"
#include "split.h"

//	   terminated operations
#include "nth.h"
#include "print_to.h"
#include "reduce.h"
#include "sum.h"
#include "to_vector.h"
#include "max.h"

namespace lipaboy_lib {

	namespace stream_space {

		//-----------------------------------------------------------//
		//-----------------------------------------------------------//
		//--------------Issues for writing own operator--------------//
		//-----------------------------------------------------------//
		//-----------------------------------------------------------//
		//
		// 1) You must find out if your operator would be terminated or not.
		//		- Terminated operator terminates (finishes) the stream and
		//		produces smth.
		//		- Non-terminated operator extends the stream by creating new 
		//		interlayer of processing the stream data
		//
		//		If your operator is terminated than you must add the 
		//		boolean 'isTerminated' compile-time class property with true value:
		//			static constexpr bool isTerminated = true;
		//		Else:
		//			static constexpr bool isTerminated = false;
		//
		// 2) In operator class must be defined the RetType template alias type 
		//		which says what's type
		//		your operator will return by applying it. Template argument is type of
		//		data that your operator processes.
		//
		//		template <class T>
		//		using RetType = YourOperatorReturnType<T>;
		//
		//		Example:
		//		int sum1 = Stream(1, 2, 3, 4, 5) | your_operator() | sum();
		//		Stream{int} -> Stream{YourOperatorReturnType<int>} -> int
		//
		// 3) For non-terminated operator in your operator class must be implemented 
		//		Stream API - three template methods:
		//
		//		A. nextElem - returns next element of stream data
		//
		//			template <class StreamType>
		//			auto nextElem<StreamType>(StreamType & stream) -> SomeReturnType;
		//
		//		B. incrementSlider - like nextElem but without returning the next element
		//
		//			template <class StreamType>
		//			void incrementSlider<StreamType>(StreamType & stream);
		//
		//		C. hasNext - says true if your processed stream has next element
		//
		//			template <class StreamType>
		//			bool hasNext<StreamType>(StreamType & stream);
		//
		//	Advice: you can call Stream API (from 3rd paragraph) of stream that is processed.
		//
		// 4) For terminated operator must be implemented one template method 'apply':
		//
		//		template <class StreamType>
		//		auto apply(StreamType & stream) -> SomeReturnType;
		//
		// Your instruments:
		//	- stream.hasNext();
		//	- stream.nextElem();
		//	- stream.incrementSlider();
		//	- typename StreamType::ResultValueType;
		//
		// Difference between non-terminated and terminated operators:
		//		1) one-to-one vs all-to-one
		//			+ many-to-one vs all-to-one
		//		2) in terminated operator method 'apply' processes the whole stream and 
		//			produces end result.
		//		3) non-terminated operator processes the stream and produces the stream

		// Example operator
		namespace operators {

			// Example of applying this operator you can find: tests/stream/nop_tests.cpp

			// Non-terminated operator
			class nop {
			public:
				static constexpr bool isTerminated = false;

				template <class T>
				using RetType = T; // return the same type

			public:
				template <class StreamType>
				auto nextElem(StreamType & stream) 
					-> RetType<typename StreamType::ResultValueType> 
				{
					return stream.nextElem();
				}

				template <class StreamType>
				void incrementElem(StreamType & stream)
				{
					stream.incrementElem();
				}

				template <class StreamType>
				bool hasNext(StreamType & stream)
				{
					return stream.hasNext();
				}
			};

			// Terminated operator
			class return_zero {
			public:
				static constexpr bool isTerminated = true;

				template <class T>
				using RetType = int;

			public:
				template <class StreamType>
                auto apply(StreamType & stream)
                    -> RetType<typename StreamType::ResultValueType>
				{
					while (stream.hasNext()) {
						auto elem = stream.nextElem();
					}
					return 0;
				}
			};

		}


		namespace experimental {

			// smth interesting

			template <class T, class RelativeTo>
			struct relative_const {
				using type = T;
			};

			template <class T, class RelativeTo>
			struct relative_const<T, const RelativeTo> {
				using type = const T;
			};

			template <class T, class RelativeTo>
			using relative_const_t = typename relative_const<T, RelativeTo>::type;

		}

	}

}

