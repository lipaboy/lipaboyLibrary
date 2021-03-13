#include <iostream>
#include <vector>
#include <string>

#include <functional>

#include <gtest/gtest.h>

#include "stream_test.h"
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

	TEST_F(PrepareStreamTest, Nth_first_elem) {
		auto res = Stream(begin(), end())
			| nth(0);
		auto res2 = Stream(begin(), end())
			| map([](typename PrepareStreamTest::ElemType a) { return a; })
			| nth(0);

		ASSERT_EQ(res, (*pOutsideContainer)[0]);
		ASSERT_EQ(res2, (*pOutsideContainer)[0]);
	}

	TEST_F(PrepareStreamTest, Nth_last_elem) {
		auto res = Stream(begin(), end())
			| map([](typename PrepareStreamTest::ElemType a) { return a; })
			| nth(pOutsideContainer->size() - 1);

		ASSERT_EQ(res, pOutsideContainer->back());
	}

	TEST_F(PrepareStreamTest, Nth_out_of_range) {
		ASSERT_FALSE((Stream(begin(), end()) | nth(pOutsideContainer->size())).has_value());
	}

	TEST_F(PrepareStreamTest, Nth_out_of_range_by_negative_index) {
		ASSERT_FALSE((Stream(begin(), end()) | nth(-1)).has_value());
	}

	TEST_F(PrepareStreamTest, Nth_out_of_range_in_extended_stream) {
		auto res = Stream(begin(), end())
			| map([](typename PrepareStreamTest::ElemType a) { return a; })
			| nth(pOutsideContainer->size())
			| or_else(-1);
		ASSERT_EQ(res, -1);
	}

}

