#include <gtest/gtest.h>

#include <string>
#include <iostream>

#include "long_numbers/long_integer.h"

namespace long_numbers_tests {

	using namespace lipaboy_lib;
	using namespace lipaboy_lib::long_numbers_space;

	using std::cout;
	using std::endl;
	using std::string;


	//-------------------------------------------------------------------//
	//----------------------------to_string()----------------------------//
	//-------------------------------------------------------------------//

	TEST(LongInteger, sign) {
		LongInteger<1> first(-5);
		ASSERT_TRUE(first.sign() < 0);
		
		LongInteger<1> second(0);
		ASSERT_TRUE(second.sign() == 0);
	}

	TEST(LongInteger, revert_sign) {
		LongInteger<1> first(-5);
		first = -first;
		ASSERT_TRUE(first.sign() > 0);

		LongInteger<1> second(0);
		second = -second;
		ASSERT_TRUE(second.sign() == 0);
	}

	TEST(LongInteger, copy_constructor_different_length) {
		LongInteger<1> first(-5);
		LongInteger<2> second(first);

		ASSERT_TRUE(second.sign() < 0);
	}

	TEST(LongInteger, construct_from_string) {
		LongInteger<1> first("  -5");
		ASSERT_TRUE(first.sign() < 0);

		LongInteger<2> second("123456789012");
		ASSERT_EQ(second.to_string(), "123456789012");
		second = -second;
		ASSERT_EQ(second.to_string(), "-123456789012");
	}

}
