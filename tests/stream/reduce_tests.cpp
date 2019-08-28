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

	TEST(Stream_Reduce, Infinite) {
		int a = 0;
		auto res = buildStream([&a]() { return a++; })
			| get(4)
			| reduce([](int res, int elem) { return res + elem; });
		ASSERT_EQ(res, 6);
	}

	TEST(Stream_Sum, Infinite) {
		int a = 0;
		auto res = buildStream([&a]() { return a++; })
			| get(4)
			| sum();
		ASSERT_EQ(res, 6);
	}

	TEST(Stream_Sum, Empty_int) {
		int a = 1;
		auto res = buildStream([&a]() { return a++; })
			| get(0)
			| sum(-1);
		ASSERT_EQ(res, -1);
	}

	TEST(Stream_Sum, Empty_string) {
		string a = "";
		auto res = buildStream([&a]() { return a + "2"; })
			| get(0)
			| sum("a");
		ASSERT_EQ(res, "a");
	}

}

