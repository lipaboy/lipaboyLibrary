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

}
