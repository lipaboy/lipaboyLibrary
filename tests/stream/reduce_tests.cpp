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
	using namespace lipaboy_lib::stream_space::op;

	//---------------------------------Tests-------------------------------//

	namespace {
		int sumFunc(int res, int elem) {
			return res + elem;	
		}

		int sumFunc2(int res) {
			return res + 0;
		}
	}

	//TEST(Stream_Reduce, check_err) {
	//	/*using namespace lipaboy_lib::stream_space::shortening;
	//	static_assert(GetArgumentCount<std::function<int(int)> > == 2,
	//		"Stream.Reduce Error: count arguments of lambda \
	//						function is not equal to 2.");*/
	//	int a = 0;
	//	auto res = Stream([&a]() { return a++; })
	//		| get(4)
	//		//| reduce<std::function<int(int)> >(std::function<int(int)>([](int res) { return res + 1; }));
	//		| reduce(sumFunc2);
	//	ASSERT_EQ(res, 6);
	//}

	TEST(Stream_Reduce, Producing_Iterator) {
		int a = 0;
		auto res = Stream([&a]() { return a++; })
			| get(4)
			| reduce<std::function<int(int, int)> >(
				std::function<int(int, int)>([](int res, int elem) -> int { return res + elem; }));
		ASSERT_EQ(res, 6);
	}

	TEST(Stream_Reduce, not_lambda_as_parameter) {
		int a = 0;
		auto res = Stream([&a]() { return a++; })
			| get(4)
			| reduce(sumFunc);
		ASSERT_EQ(res, 6);
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
            | sum();
		ASSERT_EQ(res, 6);
	}

	TEST(Stream_Sum, Empty_int) {
		int a = 1;
		auto res = Stream([&a]() { return a++; })
			| get(0)
			| sum();
		ASSERT_FALSE(res.has_value());
	}

	TEST(Stream_Sum, Empty_string) {
		string a = "";
		auto res = Stream([&a]() { return a + "2"; })
			| get(0)
			| sum();
		ASSERT_FALSE(res.has_value());
	}

}

