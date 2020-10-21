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
        LongUnsigned<2> null = 4u;
		null.shiftLeft(1);
		EXPECT_EQ(null, LongUnsigned<2>(8));

		LongUnsigned<2> first = 5,
			second = 6;

		EXPECT_EQ((first + second).to_string(), "11");

		LongUnsigned<2> third = 2,
			forth = 1;

		EXPECT_EQ((third + forth).to_string(3), "10");

		third = 2;
		for (int i = 0; i < 14; i++)
			third *= LongUnsigned<2>(2);
		EXPECT_EQ(third.to_string(3), "1122221122");

		first = 2000000000;
		EXPECT_EQ(first.to_string(), "2000000000");
		first *= LongUnsigned<2>(4);
		EXPECT_EQ(first.to_string(), "8000000000");
        first -= LongUnsigned<2>(1);
        first /= LongUnsigned<2>(4);
        EXPECT_EQ(first.to_string(), "1999999999");

		first = "789100000200";
		EXPECT_EQ(first.to_string(), "789100000200");
	}

	//-----------Crash-----------//

	TEST(LongUnsigned, overflow) {
		ASSERT_NO_THROW(LongUnsigned<1> num1("789100000200"));
	}

	//-----------Comparison-----------//

	TEST(LongUnsigned, comparison_less) {
		LongUnsigned<3> first("789100000200");
		LongUnsigned<3> second("789100000201");

		EXPECT_TRUE(first < second);

		first = "789100000200";
		second = "790100000200";

		EXPECT_TRUE(first < second);
	}

	TEST(LongUnsigned, comparison_more) {

		LongUnsigned<2> first("200200");
		LongUnsigned<2> second("200200");

		EXPECT_TRUE((first * second) > first * LongUnsigned<2>(1000));

	}

	TEST(LongUnsigned, comparison_diff) {
		LongUnsigned<3> first;
		LongUnsigned<3> second;

		first = 15;
		second = 16;
		EXPECT_TRUE(first < second);
		first = 15;
		second = 16;
		EXPECT_TRUE(first <= second);
		first = 16;
		second = 16;
		EXPECT_FALSE(first > second);
		first = 16;
		second = 16;
		EXPECT_TRUE(first >= second);
	}

	TEST(LongUnsigned, comparison_different_length) {
		LongUnsigned<3> first("1");
		LongUnsigned<1> second("3");

		ASSERT_TRUE(first < second);

		first = 0;
		EXPECT_FALSE(first > LongUnsigned<1>(0));
		EXPECT_FALSE(first < LongUnsigned<1>(0));
		EXPECT_TRUE(first <= LongUnsigned<1>(0));

		first = "20000000000000000000000";
		second = "0";
		EXPECT_TRUE(first > second);

		first = "20000000000000";
		second = "23452";
		EXPECT_TRUE(first > second);
		EXPECT_FALSE(first < second);
		EXPECT_TRUE(second < first);

		first = "20000";
		second = "23452";
		EXPECT_FALSE(first > second);
		EXPECT_TRUE(first < second);
		EXPECT_FALSE(second < first);

		LongUnsigned<2> third("20000000000000");
		EXPECT_TRUE(first < third);
		EXPECT_TRUE(first < third);
		EXPECT_FALSE(third < first);
	}

	//-----------Equality-----------//

	TEST(LongUnsigned, inequality) {
		LongUnsigned<3> num1("00001000789100000200");
		LongUnsigned<3> num2("789100000200");

		ASSERT_NE(num1, num2);

		LongUnsigned<3> num3("00000000789100000200");
		LongUnsigned<3> num4("789100000200");

		ASSERT_EQ(num3, num4);
	}

	TEST(LongUnsigned, equality) {
		LongUnsigned<2> num1("789100000200");
		LongUnsigned<2> num2("789100000200");

		ASSERT_EQ(num1, num2);
	}

    //---------Operator/ checking-----------//

    TEST(LongUnsigned, division_double) {
        LongUnsigned<2> first("200200");
        LongUnsigned<2> second("200200");

        EXPECT_EQ((first / second).to_string(), "1");

		first *= second;

        EXPECT_EQ((first / second).to_string(), "200200");
    }

	TEST(LongUnsigned, division_single) {
		LongUnsigned<2> first("1");
		LongUnsigned<2> second("200200");

		EXPECT_EQ((first / second).to_string(), "0");

		first *= second;

		EXPECT_EQ((first / second).to_string(), "1");
	}

	TEST(LongUnsigned, division_filled_number) {
		LongUnsigned<2> first(2);
		LongUnsigned<2> second(6);

		first -= second;
		ASSERT_NO_THROW(first / LongUnsigned<2>(10));

		first = LongUnsigned<2>(0) - LongUnsigned<2>(1);
		first /= (first - LongUnsigned<2>(1));
		ASSERT_EQ(first.to_string(), "1");
	}

    //---------Operator% checking-----------//

    TEST(LongUnsigned, remainder_double) {
        LongUnsigned<2> first("200201");
        LongUnsigned<2> second("200200");

        EXPECT_EQ((first % second).to_string(), "1");

		first *= second;

        EXPECT_EQ((first % second).to_string(), "0");
    }

	TEST(LongUnsigned, remainder_filled_number) {
		LongUnsigned<2> first(0);
		first -= LongUnsigned<2>(1);
		first %= (first - LongUnsigned<2>(1));
		ASSERT_EQ(first.to_string(), "1");
	}

    //---------Operator* checking-----------//

    TEST(LongUnsigned, multiplication_by_different_lengths) {
        LongUnsigned<1> num1("2");
        LongUnsigned<4> num2("10000010000");

        EXPECT_EQ((num1 * num2).to_string(), "20000020000");

        LongUnsigned<4> num3("2");
        LongUnsigned<4> num4("10000010000");

        for (int i = 0; i < 25; i++) {
            num2 *= num1;
            num4 *= num3;
        }
        ASSERT_EQ(num2.to_string(), num4.to_string());
    }

    TEST(LongUnsigned, multiplication_double_rank_by_independent_parts) {
        LongUnsigned<2> num3("200200");
        LongUnsigned<2> num4("200200");

        EXPECT_EQ((num3 * num4).to_string(), "40080040000");

        num3 *= num4;

        EXPECT_EQ(num3.to_string(), "40080040000");

        LongUnsigned<1> num5("10");
        EXPECT_EQ((num4 * num5).to_string(), "2002000");
    }

    TEST(LongUnsigned, multiplication_simple) {
        LongUnsigned<1> num1("2");
        LongUnsigned<1> num2("2");

        ASSERT_EQ((num1 * num2).to_string(), "4");

        LongUnsigned<2> num3("200000");
        LongUnsigned<2> num4("200000");

        EXPECT_EQ((num3 * num4).to_string(), "40000000000");
    }

	//---------Operator- checking-----------//

	TEST(LongUnsigned, substract_different_rank) {
		LongUnsigned<3> first(1);
		LongUnsigned<1> second(1);
		first.shiftLeft(first.integralModulusDegree() * 3 - 1);
		first -= second;
		ASSERT_EQ(string(first.integralModulusDegree() * 3 - 1, '1'), first.to_string(2));
	}

	TEST(LongUnsigned, substract_overflow) {
		LongUnsigned<2> first(2);
		LongUnsigned<2> second(6);

		ASSERT_NO_THROW(first -= second);
	}

	TEST(LongUnsigned, substract_double_rank) {
		LongUnsigned<2> first (std::numeric_limits<uint32_t>::max());
		LongUnsigned<2> second(6);
		first += LongUnsigned<2>(3);
		ASSERT_EQ(std::to_string(std::numeric_limits<uint32_t>::max() - 3u), 
			(first - second).to_string());
	}

	TEST(LongUnsigned, substract_single_rank) {
		LongUnsigned<1> first ("101000200");
		LongUnsigned<1> second("  1000001");

		ASSERT_EQ("100000199", (first - second).to_string());
	}

    //---------Operator+ checking-----------//

	TEST(LongUnsigned, sum_different_length) {
		LongUnsigned<3> first (std::numeric_limits<uint32_t>::max() - 3);
		LongUnsigned<1> second(6);

		first += second;
		EXPECT_EQ(std::to_string(uint64_t(std::numeric_limits<uint32_t>::max()) + uint64_t(3u)), 
			first.to_string());

		first = LongUnsigned<3>(1);
		second = LongUnsigned<1>(1);
		first.shiftLeft(2 * first.integralModulusDegree());
		first -= second;
		first += second;
		EXPECT_EQ("1" + string(2 * first.integralModulusDegree(), '0'), first.to_string(2));
		EXPECT_EQ("1" + string(2 * first.integralModulusDegree(), '0'), ((first - second) + second).to_string(2));
	}

    TEST(LongUnsigned, sum_triple_rank_simple) {
        LongUnsigned<3> num1("22000000789100000200");
        LongUnsigned<3> num2("23000000111901000001");

        ASSERT_EQ("45000000901001000201", (num1 + num2).to_string());
        ASSERT_EQ("45000000901001000201", (num2 + num1).to_string());
    }


    TEST(LongUnsigned, sum_double_rank_by_crossing_parts) {
        LongUnsigned<2> num1("789100000200");
        LongUnsigned<2> num2("111901000001");

        ASSERT_EQ("901001000201", (num1 + num2).to_string());
        ASSERT_EQ("901001000201", (num2 + num1).to_string());
    }

    TEST(LongUnsigned, sum_double_rank_by_independent_parts) {
        LongUnsigned<2> num1("789100000200");
        LongUnsigned<2> num2("111001000001");

        ASSERT_EQ("900101000201", (num1 + num2).to_string());
    }

    TEST(LongUnsigned, sum_single_rank) {
        LongUnsigned<1> num1("100000200");
        LongUnsigned<1> num2("  1000001");

        ASSERT_EQ("101000201", (num1 + num2).to_string());
    }

	//---------Operator shift-----------//

	TEST(LongUnsigned, shift_single_rank) {
		LongUnsigned<1> first("100000200");
		first.shiftLeft(1);
		ASSERT_EQ("200000400", first.to_string());
		first.shiftRight(1);
		ASSERT_EQ("100000200", first.to_string());
	}

	TEST(LongUnsigned, shift_double_rank) {
		LongUnsigned<2> first("1000002000");
		first.shiftLeft(3);
		ASSERT_EQ("8000016000", first.to_string());
		first.shiftRight(3);
		ASSERT_EQ("1000002000", first.to_string());
	}

	TEST(LongUnsigned, shift_triple_rank) {
		LongUnsigned<3> first(2);
		first.shiftLeft(first.integralModulusDegree());
		EXPECT_EQ("2" + string(first.integralModulusDegree() / 2, '0'), first.to_string(4));
		first.shiftRight(first.integralModulusDegree());
		EXPECT_EQ("2", first.to_string(4));
	}

	//---------Other-----------//

	TEST(LongUnsigned, major_bit_position_single_rank) {
		LongUnsigned<1> first(std::numeric_limits<uint32_t>::max());
		EXPECT_EQ(8 * sizeof(typename LongUnsigned<1>::IntegralType) - 1, first.majorBitPosition());
		first.shiftRight(1);
		EXPECT_EQ(8 * sizeof(typename LongUnsigned<1>::IntegralType) - 2, first.majorBitPosition());
	}

	TEST(LongUnsigned, major_bit_position_double_rank) {
		LongUnsigned<2> first(std::numeric_limits<uint32_t>::max());
		first.shiftLeft(1);
		EXPECT_EQ(8 * sizeof(typename LongUnsigned<2>::IntegralType), first.majorBitPosition().value());
		first.shiftRight(1);
		EXPECT_EQ(8 * sizeof(typename LongUnsigned<2>::IntegralType) - 1, first.majorBitPosition().value());

		first = 0;
		EXPECT_FALSE(first.majorBitPosition().has_value());
		first += LongUnsigned<2>(1);
		EXPECT_EQ(0, first.majorBitPosition().value());
	}

}

