#pragma once

#include "tools.h"

// non-terminated operators
#include "filter.h"
#include "group_by_vector.h"
#include "get.h"
#include "skip.h"
#include "ungroup_by_bit.h"
#include "map.h"
#include "distinct.h"
#include "split.h"
#include "cast.h"
//     special operators
#include "to_pair.h"
#include "tupled.h"

//	   terminated operators
#include "nth.h"
#include "print_to.h"
#include "reduce.h"
#include "sum.h"
#include "to_vector.h"
#include "max.h"

namespace lipaboy_lib::stream_space {

	//-----------------------------------------------------------//
	//-----------------------------------------------------------//
	//--------------Issues for writing own operator--------------//
	//-----------------------------------------------------------//
	//-----------------------------------------------------------//
	//
	// 1) You must find out if your operator would be terminated or not.
	//		- Terminated operator terminates (finishes) the data stream and
	//		always produces smth directly (like value) or indirectly (like printing into console).
	//		- Non-terminated operator extends the stream by creating new 
	//		interlayer of processing the stream data.
	//
	//		If your operator is terminated than you must inherit it from "TerminatedOperator" class
	//		(located at "stream/operators/tools.h")
	//		class YourOperator : TerminatedOperator {}
	//
	// 2) In operator class must be defined the "RetType" template alias type 
	//		which says what's type your operator will return by applying it. 
	//		Template argument of alias is type of
	//		data that your operator processes.
	//
	//		> template <class T>
	//		> using RetType = YourOperatorReturnType<T>;
	//
	//		Example:
	//		> int sum1 = Stream(1, 2, 3, 4, 5) | your_operator() | sum();
	//		your_operator input type: int
	//		your_operator output type: YourOperatorReturnType<int> ("==" RetType<int>)
	//
	//		Whole map of types:
	//		std::initializer_list<int> -> Stream{ElemType: int} -> Stream{YourOperatorReturnType<int>} -> int
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
	//		C. hasNext - says true only if your processed stream has next element
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
	// Your instruments (par. 3 and 4):
	//	- stream.hasNext();
	//	- stream.nextElem();
	//	- stream.incrementSlider();
	//	- typename StreamType::ResultValueType;
	//
	// Difference between non-terminated and terminated operators:
	//		1) Style of transforming data elements:
	//			Non-term.:	one-to-one, many-to-one, one-to-many
	//			Term.:		all-to-one
	//		2) In terminated operator method 'apply' processes the whole stream and 
	//			produces end result.
	//		3) Non-terminated operator processes the stream and produces a performed stream.

	// Example Of Operators
	namespace operators {

		// Example of applying this operator you can find: tests/stream/nop_tests.cpp

		// Non-terminated operator
		class nop {
		public:
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
		class return_zero : TerminatedOperator
		{
        public:
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

}

