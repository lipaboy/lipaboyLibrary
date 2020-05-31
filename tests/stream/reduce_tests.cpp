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

	using namespace lipaboy_lib::stream_space;
	using namespace lipaboy_lib::stream_space::operators;

	//---------------------------------Tests-------------------------------//

	namespace {
		int sumFunc(int res, int elem) {
			return res + elem;	
		}
	}

	TEST(Stream_Reduce, Infinite) {
		int a = 0;
		auto res = Stream([&a]() { return a++; })
			| get(4)
			| reduce([](int res, int elem) { return res + elem; });
		ASSERT_EQ(res, 6);

		/*a = 0;
		res = Stream([&a]() { return a++; })
			| get(4)
			| reduce(sumFunc);
		ASSERT_EQ(res, 6);*/
	}

	TEST(Stream_Reduce, Char_sticking) {
		vector<char> vec{ 'l', 'o', 'l', ' ', 'k', 'e', 'k' };
		auto res = Stream(vec.begin(), vec.end())
			| reduce([](string const & res, char elem) { return res + string(1, elem); },
				[](char elem) -> string { return string(1, elem); });
		ASSERT_EQ(res, "lol kek");
	}

	TEST(Stream_Reduce, Empty) {
		vector<char> vec{ 'l', 'o', 'l', ' ', 'k', 'e', 'k' };
		auto res = Stream(vec.begin(), vec.end())
			| get(0)
			| reduce([](string const & res, char elem) { return res + string(1, elem); },
				[](char elem) -> string { return string(1, elem); });
		ASSERT_EQ(res.has_value(), false);
	}

	TEST(Stream_Sum, Infinite) {
		int a = 0;
		auto res = Stream([&a]() { return a++; })
			| get(4)
            | sum<>();
		ASSERT_EQ(res, 6);
	}

	TEST(Stream_Sum, Empty_int) {
		int a = 1;
		auto res = Stream([&a]() { return a++; })
			| get(0)
			| sum(-1);
		ASSERT_EQ(res, -1);
	}

	TEST(Stream_Sum, Empty_string) {
		string a = "";
		auto res = Stream([&a]() { return a + "2"; })
			| get(0)
			| sum("a");
		ASSERT_EQ(res, "a");
	}

}

