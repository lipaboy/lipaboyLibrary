#include <gtest/gtest.h>

#include <string>
#include <iostream>

#include "long_numbers/long_unsigned.h"

namespace long_numbers_tests {

	using namespace lipaboy_lib;

	using namespace long_numbers_space;
	using std::cout;
	using std::endl;
	using std::string;

	//-----Any system------//

	TEST(LongUnsigned, simple) {
		LongUnsigned<2, 10> first = 5,
			second = 6;

		EXPECT_EQ((first + second).to_string(), "11");

		LongUnsigned<2, 3> third = 2,
			forth = 1;

		EXPECT_EQ((third + forth).to_string(), "10");
		EXPECT_EQ(third.integralModulusDegree(), 20);
		EXPECT_EQ(third.integralModulus(), 3486784401);

		third = 2;
		for (int i = 0; i < 14; i++)
			third *= LongUnsigned<2, 3>(2);
		EXPECT_EQ(third.to_string(), "1122221122");
	}

}

