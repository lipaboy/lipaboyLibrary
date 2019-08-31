#include <iostream>
#include <vector>
#include <string>

#include <functional>

#include <gtest/gtest.h>

#include "stream/stream.h"

namespace stream_tests {

	using std::cout;
	using std::endl;
	using std::vector;
	using std::string;

	using namespace lipaboy_lib;

	using namespace lipaboy_lib::stream;
	using namespace lipaboy_lib::stream::operators;

	//---------------------------------Tests-------------------------------//

	TEST(Stream_max, simple) {
		int a = 0;
		auto max1 = buildStream([&a]() { return a++; }) 
			| get(1000) 
			| max();
		ASSERT_EQ(max1, 999);
	}

	TEST(Stream_max, l_value) {
		int a = 0;
		auto maxOp = max();
		auto max1 = buildStream([&a]() { return a++; })
			| get(1000)
			| maxOp;
		ASSERT_EQ(max1, 999);
	}

	TEST(Stream_max, empty) {
		int a = 0;
		auto max1 = buildStream([&a]() { return a++; }) 
			| get(0) 
			| max();

		ASSERT_EQ(max1.has_value(), false);
	}

}

