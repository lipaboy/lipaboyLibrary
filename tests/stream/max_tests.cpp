#include <iostream>
#include <vector>
#include <string>

#include <functional>

#include <gtest/gtest.h>

#include "stream/stream.h"
#include "maths/fixed_precision_number.h"

namespace stream_tests {

	using std::cout;
	using std::endl;
	using std::vector;
	using std::string;

	using namespace lipaboy_lib;

	using namespace lipaboy_lib::stream_space;
	using namespace lipaboy_lib::stream_space::operators;

	//---------------------------------Tests-------------------------------//

	TEST(Stream_max, simple) {
		int a = 0;
		auto max1 = Stream([&a]() { return a++; }) 
			| get(1000) 
			| max();
		ASSERT_EQ(max1, 999);
	}

	TEST(Stream_max, l_value) {
		int a = 0;
		auto maxOp = max();
		auto max1 = Stream([&a]() { return a++; })
			| get(1000)
			| maxOp;
		ASSERT_EQ(max1, 999);
	}

	TEST(Stream_max, empty) {
		int a = 0;
		auto max1 = Stream([&a]() { return a++; }) 
			| get(0) 
			| max();

		ASSERT_EQ(max1.has_value(), false);
	}

	TEST(Stream_max, fixed_precision_numbers) {
		using FixedDouble = lipaboy_lib::FixedPrecisionNumber<double, 1, -3>;

		FixedDouble a(0.);
		auto max1 = Stream([&a]() { return (a = a + 1.); })
			| get(10)
			| max();

		ASSERT_EQ(max1.value(), 10.);
	}

}

