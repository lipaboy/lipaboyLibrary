#include <iostream>
#include <vector>
#include <string>

#include <gtest/gtest.h>

#include "stream_test.h"

namespace stream_tests {

	using std::cout;
	using std::endl;
	using std::vector;
	using std::string;

	using namespace lipaboy_lib::stream_space;
	using namespace lipaboy_lib::stream_space::operators;


	TEST(Stream_Cast, simple) {
		char a = 0;
		auto res = Stream([&a]() { return a++; })
			| cast_to<int>()
			| get(5)
			| sum();

		ASSERT_EQ(res, 10);

		a = 0;
		res = Stream([&a]() { return a++; })
			| cast_static<int>()
			| get(5)
			| sum();

		ASSERT_EQ(res, 10);
	}


}

