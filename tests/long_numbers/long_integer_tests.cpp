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

    TEST(LongInteger, check) {

    }

    TEST(LongInteger, sign) {
        LongInteger<1> first(-5);
        EXPECT_TRUE(first.sign() < 0);
		
        LongInteger<1> second(0);
        EXPECT_TRUE(second.sign() == 0);
    }

    TEST(LongInteger, operator_minus) {
        LongInteger<1> first(-5);
        first = -first;
        EXPECT_TRUE(first.sign() > 0);

        LongInteger<1> second(0);
        second = -second;
        EXPECT_TRUE(second.sign() == 0);
    }

    TEST(LongInteger, invert) {
        LongInteger<1> first(-5);
        first.invert();
        EXPECT_TRUE(first.sign() > 0);

        LongInteger<1> second(0);
        second.invert();
        EXPECT_TRUE(second.sign() == 0);
    }

    TEST(LongInteger, copy_constructor_different_length) {
        LongInteger<1> first(-5);
        LongInteger<2> second(first);

        EXPECT_TRUE(second.sign() < 0);

        first = -first;
        LongInteger<3> third(first);

        EXPECT_TRUE(third.sign() > 0);
    }

    TEST(LongInteger, to_string) {
        LongInteger<1> first(-5);
        EXPECT_EQ(first.to_string(), "-5");
        EXPECT_EQ(first.to_string(2), "-101");
        first = -first;
        EXPECT_EQ(first.to_string(), "5");
        EXPECT_EQ(first.to_string(2), "101");

        LongInteger<2> second(first);
        EXPECT_EQ(second.to_string(), "5");
        second = -second;
        EXPECT_EQ(second.to_string(), "-5");
    }

    TEST(LongInteger, construct_from_string) {
        LongInteger<1> first("  -5");
        EXPECT_TRUE(first.sign() < 0);

        LongInteger<2> second("123456789012");
        EXPECT_EQ(second.to_string(), "123456789012");
        second = -second;
        EXPECT_EQ(second.to_string(), "-123456789012");

        LongInteger<2> third("3000000001");
        EXPECT_EQ(third.to_string(), "3000000001");
    }

    TEST(LongInteger, sum_single_lengths) {
        LongInteger<1> first(1);
        LongInteger<1> second(2);
        EXPECT_EQ((first += second).to_string(), "3");

        first.assignStr("-2");
        EXPECT_EQ((first + second).to_string(), "0");
    }

    TEST(LongInteger, sum_double_lengths) {
        LongInteger<2> first(" 3000000001");
        LongInteger<2> second("3000000001");
        EXPECT_EQ((first += second).to_string(), "6000000002");

        second = -second;
        EXPECT_EQ((first += second).to_string(), "3000000001");
        EXPECT_EQ((first + second).to_string(), "0");

        first = 0;
        EXPECT_EQ((first + second).to_string(), "-3000000001");
    }

    TEST(LongInteger, sum_diff_lengths) {
        LongInteger<2> first(" 100000000000000000000000000000001", 2);
        LongInteger<1> second("-1", 2);
        EXPECT_EQ((first += second).to_string(2), "100000000000000000000000000000000");
        EXPECT_EQ((first += second).to_string(2), LongUnsigned<1>(-1).to_string(2));
    }

    TEST(LongInteger, sub_single_lengths) {
        LongInteger<1> first(3);
        LongInteger<1> second(2);
        EXPECT_EQ((first -= second).to_string(), "1");

        first.assignStr("2");
        EXPECT_EQ((first - second).to_string(), "0");
        EXPECT_EQ((first - second - second).to_string(), "-2");
    }

    TEST(LongInteger, sub_double_lengths) {
        LongInteger<2> first(" 3000000001");
        LongInteger<2> second("3000000001");
        EXPECT_EQ((first -= second).to_string(), "0");

        second = -second;
        EXPECT_EQ((first -= second).to_string(), "3000000001");
        EXPECT_EQ((first - second).to_string(), "6000000002");

        first = 0;
        EXPECT_EQ((first - second).to_string(), "3000000001");
    }

    TEST(LongInteger, sub_diff_lengths) {
        LongInteger<2> first(" 100000000000000000000000000000001", 2);
        LongInteger<1> second("1", 2);
        EXPECT_EQ((first -= second).to_string(2), "100000000000000000000000000000000");
        EXPECT_EQ((first -= second).to_string(2), LongUnsigned<1>(-1).to_string(2));
    }

}
