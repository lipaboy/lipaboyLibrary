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

	TEST(Stream_Filter, empty) {
		int i = 0;
		auto res = buildStream(1, 2, 3)
			| filter([](auto) { return (false); })
			| to_vector();

		ASSERT_EQ(res, vector<int>());
	}

	TEST(Stream_Filter, sample) {
		int i = 0;
		auto res = buildStream(1, 2, 3)
			| filter([](auto x) { return (x == x); })
			| to_vector();

		ASSERT_EQ(res, vector<int>({ 1, 2, 3 }));
	}
	TEST(Stream_Filter, mod3) {
		auto res = buildStream(1, 2, 3, 4, 5, 6)
			| filter([](int x) { return x % 3 == 0; })
			| to_vector();

		ASSERT_EQ(res, vector<int>({ 3, 6 }));
	}

	int sqr(int a) { return a * a; }
	bool kekBool(int a) { return a % 2 == 0; }

	TEST(Stream_Filter, vector_int) {
		vector<int> olala = { 1, 2, 3, 4, 5, 6, 7, 8 };
		auto stream2 = buildStream(olala.begin(), olala.end());
		auto stream4 = stream2 | filter(kekBool)
			| map(sqr)
			| filter([](int a) -> bool { return a % 10 == 6; });
		auto kek = stream4 | to_vector();

		ASSERT_EQ(kek, decltype(kek)({ 16, 36 }));
	}

	TEST(Stream_Filter, group_by_vector) {
		vector<int> olala = { 1, 2, 3, 4, 5, 6, 7, 8 };
		auto stream2 = buildStream(olala.begin(), olala.end());
		auto kek = stream2
			| filter([](auto a) { return a % 2 == 0; })
			| group_by_vector(3)
			| to_vector();

		ASSERT_EQ(kek, decltype(kek)({ vector<int>({ 2, 4, 6 }), vector<int>({ 8 }) }));
	}

}

