#include <gtest/gtest.h>

#include <string>
#include <iostream>

#include "long_numbers/long_number.h"

namespace long_numbers_tests {

using namespace lipaboy_lib;

using long_numbers_space::LongIntegerDecimal;
using std::cout;
using std::endl;
using std::string;

TEST(LongInteger, check) {
	string f = "8123456789012345690";
	string s = "1";
	LongIntegerDecimal<4> num1 = f;
	LongIntegerDecimal<4> num2 = s;

	string res = f;
	res[res.size() - 1] = s[0];

	ASSERT_EQ(res, (num1 + num2).to_string());

}

}
