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

//-----------Crash-----------//

TEST(LongInteger, overflow) {
	ASSERT_NO_THROW(LongIntegerDecimal<1> num1("789100000200"));
}

//-----------Sign-----------//

TEST(LongInteger, sign) {
	LongIntegerDecimal<1> num(-2);

	ASSERT_EQ(num.sign(), -1);
}

//-----------Equality-----------//

TEST(LongInteger, inequality) {
	LongIntegerDecimal<3> num1("00001000789100000200");
	LongIntegerDecimal<3> num2("789100000200");

	ASSERT_NE(num1, num2);
}

TEST(LongInteger, equality2) {
	LongIntegerDecimal<3> num1("00000000789100000200");
	LongIntegerDecimal<3> num2("789100000200");

	ASSERT_EQ(num1, num2);
}

TEST(LongInteger, equality) {
	LongIntegerDecimal<2> num1("789100000200");
	LongIntegerDecimal<2> num2("789100000200");

	ASSERT_EQ(num1, num2);
}

//---------Operator+ checking-----------//

TEST(LongInteger, sum_double_rank_by_crossing_parts) {
	LongIntegerDecimal<2> num1("789100000200");
	LongIntegerDecimal<2> num2("111901000001");

	ASSERT_EQ("901001000201", (num1 + num2).to_string());
}

TEST(LongInteger, sum_double_rank_by_independent_parts) {
	LongIntegerDecimal<2> num1("789100000200");
	LongIntegerDecimal<2> num2("111001000001");

	ASSERT_EQ("900101000201", (num1 + num2).to_string());
}

TEST(LongInteger, sum_single_rank) {
	LongIntegerDecimal<1> num1("100000200");
	LongIntegerDecimal<1> num2("  1000001");

	ASSERT_EQ("101000201", (num1 + num2).to_string());
}

TEST(LongInteger, check) {
	string f = "8123456789012345690";
	string s = "1";
	LongIntegerDecimal<4> num1(f);
	LongIntegerDecimal<4> num2(s);

	string res = f;
	res[res.size() - 1] = s[0];

	ASSERT_EQ(res, (num1 + num2).to_string());

}

}
