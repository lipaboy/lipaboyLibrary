#include <gtest/gtest.h>

#include <string>
#include <iostream>

#include "long_numbers/long_number.h"

namespace long_numbers_tests {

using namespace lipaboy_lib;

using namespace long_numbers_space;
using std::cout;
using std::endl;
using std::string;

//-----------Crash-----------//

TEST(LongInteger, overflow) {
	ASSERT_NO_THROW(LongIntegerDecimal<1> num1("789100000200"));
}

//-----------Comparison-----------//

TEST(LongInteger, comparison_different_lengths) {
	LongIntegerDecimal<3> num1("1");
	LongIntegerDecimal<1> num2("3");

	ASSERT_TRUE(num1 < num2);
}

TEST(LongInteger, comparison_less) {
	LongIntegerDecimal<3> num1("789100000200");
	LongIntegerDecimal<3> num2("789100000201");

	EXPECT_TRUE(num1 < num2);

	LongIntegerDecimal<3> num3("789100000200");
	LongIntegerDecimal<3> num4("790100000200");

	EXPECT_TRUE(num3 < num4);

	LongIntegerDecimal<3> num5("-23");
	LongIntegerDecimal<3> num6("23");

	EXPECT_TRUE(num5 < num6);

	LongIntegerDecimal<3> num7("0");
	LongIntegerDecimal<3> num8("-0");

	EXPECT_FALSE(num7 < num8);

    LongIntegerDecimal<2> num9("200200");
    LongIntegerDecimal<2> num10("200200");

	EXPECT_TRUE((num9 * num10) > num9 * LongIntegerDecimal<2>(1000));

	num7 = -15;
	num8 = -16;
	EXPECT_TRUE(num7 > num8);
	num7 = -15;
	num8 = -16;
	EXPECT_TRUE(num7 >= num8);
	num7 = -16;
	num8 = -16;
	EXPECT_FALSE(num7 < num8);
	num7 = -16;
	num8 = -16;
	EXPECT_TRUE(num7 <= num8);
}

//-----------Sign-----------//

TEST(LongInteger, sign) {
	LongIntegerDecimal<1> num(-2);

	ASSERT_EQ(num.sign(), -1);

	LongIntegerDecimal<1> num2("-2");

	ASSERT_EQ(num2.sign(), -1);

	LongIntegerDecimal<2> num3("-790100000200");

	ASSERT_EQ(num3.sign(), -1);

	LongIntegerDecimal<2> num4("-000000000000");

	ASSERT_EQ(num4.sign(), 0);
}

//-----------Equality-----------//

TEST(LongInteger, inequality) {
	LongIntegerDecimal<3> num1("00001000789100000200");
	LongIntegerDecimal<3> num2("789100000200");

	ASSERT_NE(num1, num2);

	LongIntegerDecimal<3> num3("00000000789100000200");
	LongIntegerDecimal<3> num4("789100000200");

	ASSERT_EQ(num3, num4);
}

TEST(LongInteger, equality) {
	LongIntegerDecimal<2> num1("789100000200");
	LongIntegerDecimal<2> num2("789100000200");

	ASSERT_EQ(num1, num2);
}

//---------Operator/ checking-----------//

TEST(LongInteger, division_by_dec) {
    LongIntegerDecimal<2> num2("123456789012");
    auto remainder = num2.divideByDec();
    EXPECT_EQ(num2.to_string(), "12345678901");
    EXPECT_EQ(remainder, 2);
    remainder = num2.divideByDec();
    EXPECT_EQ(num2.to_string(), "1234567890");
    EXPECT_EQ(remainder, 1);
    remainder = num2.divideByDec();
    EXPECT_EQ(num2.to_string(), "123456789");
    EXPECT_EQ(remainder, 0);
    remainder = num2.divideByDec();
    EXPECT_EQ(num2.to_string(), "12345678");
    EXPECT_EQ(remainder, 9);
    remainder = num2.divideByDec();
    EXPECT_EQ(num2.to_string(), "1234567");
    EXPECT_EQ(remainder, 8);
    for (int i = 0; i < 6; i++)
        remainder = num2.divideByDec();
    EXPECT_EQ(num2.to_string(), "1");
    EXPECT_EQ(remainder, 2);
    remainder = num2.divideByDec();
    EXPECT_EQ(num2.to_string(), "0");
    EXPECT_EQ(remainder, 1);
    remainder = num2.divideByDec();
    EXPECT_EQ(num2.to_string(), "0");
    EXPECT_EQ(remainder, 0);
}

//---------Operator/ checking-----------//

TEST(LongInteger, division_double) {
    LongIntegerDecimal<2> num3("200200");
    LongIntegerDecimal<2> num4("200200");

    EXPECT_EQ((num3 / num4).to_string(), "1");

    num3 *= num4;

    EXPECT_EQ((num3 / num4).to_string(), "200200");
}

//---------Operator% checking-----------//

TEST(LongInteger, remainder_double) {
    LongIntegerDecimal<2> num3("200201");
    LongIntegerDecimal<2> num4("200200");

    EXPECT_EQ((num3 % num4).to_string(), "1");

    num3 *= num4;

    EXPECT_EQ((num3 % num4).to_string(), "0");
}

//---------Operator* checking-----------//

TEST(LongInteger, multiplication_by_different_lengths) {
	LongIntegerDecimal<1> num1("2");
	LongIntegerDecimal<4> num2("10000010000");
	
	EXPECT_EQ((num1 * num2).to_string(), "20000020000");

	LongIntegerDecimal<4> num3("2");
	LongIntegerDecimal<4> num4("10000010000");

	for (int i = 0; i < 25; i++) {
		num2 *= num1;
		num4 *= num3;
	}
	ASSERT_EQ(num2.to_string(), num4.to_string());
}

TEST(LongInteger, multiplication_double_rank_by_independent_parts) {
	LongIntegerDecimal<2> num3("200200");
	LongIntegerDecimal<2> num4("200200");

	EXPECT_EQ((num3 * num4).to_string(), "40080040000");

    num3 *= num4;

    EXPECT_EQ(num3.to_string(), "40080040000");

    LongIntegerDecimal<1> num5("10");
    EXPECT_EQ((num4 * num5).to_string(), "2002000");
}

TEST(LongInteger, multiplication_different_signs) {
	LongIntegerDecimal<1> num1("-2");
	LongIntegerDecimal<1> num2("2");

	ASSERT_EQ((num1 * num2).to_string(), "-4");
	ASSERT_EQ((num2 * num1).to_string(), "-4");

	LongIntegerDecimal<2> num3("-200000");
	LongIntegerDecimal<2> num4("-200000");

	EXPECT_EQ((num3 * num4).to_string(), "40000000000");
}

TEST(LongInteger, multiplication_simple) {
	LongIntegerDecimal<1> num1("2");
	LongIntegerDecimal<1> num2("2");

	ASSERT_EQ((num1 * num2).to_string(), "4");

	LongIntegerDecimal<2> num3("200000");
	LongIntegerDecimal<2> num4("200000");

	EXPECT_EQ((num3 * num4).to_string(), "40000000000");
}

// have some errors
TEST(LongInteger, DISABLED_karacuba_multiplication_simple) {
    LongIntegerDecimal<1> num1("2");
    LongIntegerDecimal<1> num2("2");

    ASSERT_EQ(num1.multiplyByKaracuba(num2), num1 * num2);

    LongIntegerDecimal<2> num3("200000");
    LongIntegerDecimal<2> num4("200000");

    EXPECT_EQ(num3.multiplyByKaracuba(num4), num3 * num4);

	LongIntegerDecimal<3> num5("2");
	LongIntegerDecimal<3> num6("200000");

	EXPECT_EQ((num5.multiplyByKaracuba(num6)).to_string(), (num5 * num6).to_string());

	LongIntegerDecimal<1> num7("-2");
	LongIntegerDecimal<1> num8("2");

	ASSERT_EQ(num7.multiplyByKaracuba(num8), num7 * num8);

	num5 = 3;
	num6 = 3;
	for (int i = 0; i < 18; i++) {
		num5 = num5.multiplyByKaracuba(decltype(num5)(3)) + 0;
		num6 = num6 * decltype(num6)(3) + 0; 
	}
	num6 = num6 * decltype(num6)(3) + 0;
	num5 = num5.multiplyByKaracuba(decltype(num5)(3)) + 0;
	
	EXPECT_EQ(num5.to_string(), num6.to_string());
}

//---------Operator- checking-----------//

TEST(LongInteger, inverse_the_number) {
	LongIntegerDecimal<1> num("2");

	ASSERT_EQ(-num, LongIntegerDecimal<1>("-2"));
}

//---------Operator+ checking-----------//

TEST(LongInteger, sum_triple_rank_simple) {
	LongIntegerDecimal<3> num1("22000000789100000200");
	LongIntegerDecimal<3> num2("23000000111901000001");

	ASSERT_EQ("45000000901001000201", (num1 + num2).to_string());
	ASSERT_EQ("45000000901001000201", (num2 + num1).to_string());
}


TEST(LongInteger, sum_double_rank_by_crossing_parts) {
	LongIntegerDecimal<2> num1("789100000200");
	LongIntegerDecimal<2> num2("111901000001");

	ASSERT_EQ("901001000201", (num1 + num2).to_string());
	ASSERT_EQ("901001000201", (num2 + num1).to_string());
    interesting::Summarize<2, 0>::sum(num1, num2, 0);
	ASSERT_EQ("901001000201", (num1).to_string());
}

TEST(LongInteger, sum_double_rank_by_independent_parts_negative) {
	LongIntegerDecimal<2> num1(" 789101000201");
	LongIntegerDecimal<2> num2("-111001000001");

	ASSERT_EQ("678100000200", (num1 + num2).to_string());
	ASSERT_EQ("678100000200", (num2 + num1).to_string());
}

TEST(LongInteger, sum_double_rank_by_independent_parts) {
	LongIntegerDecimal<2> num1("789100000200");
	LongIntegerDecimal<2> num2("111001000001");

	ASSERT_EQ("900101000201", (num1 + num2).to_string());
}

TEST(LongInteger, sum_single_rank_negative) {
	LongIntegerDecimal<1> num1("1010002");
	LongIntegerDecimal<1> num2(" -10000");

	ASSERT_EQ("1000002", (num1 + num2).to_string());
	ASSERT_EQ("1000002", (num2 + num1).to_string());

	LongIntegerDecimal<1> num3("-1010002");
	LongIntegerDecimal<1> num4("   10000");

	ASSERT_EQ("-1000002", (num3 + num4).to_string());
	ASSERT_EQ("-1000002", (num4 + num3).to_string());
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

    interesting::Summarize<4, 0>::sum(num1, num2, 0);
	ASSERT_EQ(res, (num1).to_string());


}

}
