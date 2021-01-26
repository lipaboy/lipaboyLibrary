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

	TEST(Stream_max_min, simple) {
		int a = 0;
		auto maxVal = Stream([&a]() { return a++; }) 
			| get(1000) 
			| max();
		ASSERT_EQ(maxVal, 999);

		a = 1000;
		auto minVal = Stream([&a]() { return a--; })
			| get(1000)
			| min();
		ASSERT_EQ(minVal, 1);
	}

	TEST(Stream_max_min, l_value) {
		int a = 0;
		auto maxOp = max();
		auto maxVal = Stream([&a]() { return a++; })
			| get(1000)
			| maxOp;
		ASSERT_EQ(maxVal, 999);
	}

	TEST(Stream_max_min, empty) {
		int a = 0;
		auto maxVal = Stream([&a]() { return a++; })
			| get(0) 
			| max();
		ASSERT_EQ(maxVal.has_value(), false);

		a = 0;
		auto minVal = Stream([&a]() { return a++; })
			| get(0)
			| min();
		ASSERT_EQ(minVal.has_value(), false);
	}

    // NOTE: on linux error is placed in FixedPrecisionNumber (howerer it is compiler err)

#ifdef WIN32
	TEST(Stream_max_min, fixed_precision_numbers) {
        using FixedDouble = lipaboy_lib::FixedPrecisionNumber<double, 1, -3>;

        FixedDouble a(0.);
        auto maxVal = Stream([&a]() { return (a = a + 1.); })
            | get(10)
            | max();

        ASSERT_EQ(maxVal.value(), 10.);

		a = 10.;
		auto minVal = Stream([&a]() { return (a = a - 1.); })
			| get(10)
			| min();

		ASSERT_EQ(minVal.value(), 0.);
	}
#endif

}

