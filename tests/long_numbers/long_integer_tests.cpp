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

    TEST(LongInteger, revert_sign) {
        LongInteger<1> first(-5);
        first = -first;
        EXPECT_TRUE(first.sign() > 0);

        LongInteger<1> second(0);
        second = -second;
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
    }

//	TEST(LongInteger, sum_single_lengths) {
//		LongInteger<1> first(1);
//		LongInteger<1> second(2);
//		EXPECT_EQ((first += second).to_string(), "3");

//		first.assignStr("-2");
//		EXPECT_EQ((first + second).to_string(), "0");
//	}

//	TEST(LongInteger, sum_double_lengths) {
//		LongInteger<2> first(" 3000000001");
//		LongInteger<2> second("3000000001");
//		EXPECT_EQ((first += second).to_string(), "6000000002");

//		second = -second;
//		EXPECT_EQ((first += second).to_string(), "3000000001");
//		EXPECT_EQ((first + second).to_string(), "0");

//		// CRASH TEST
//		first = 0;
////		EXPECT_EQ((first + second).to_string(), "-3000000001");
//	}

//	TEST(LongInteger, sum_diff_lengths) {
//		LongInteger<2> first(" 3000000001");
//		LongInteger<1> second(" 500000001");
//		EXPECT_EQ((first += second).to_string(), "3500000002");

//		second = -second;
//		EXPECT_EQ((first += second).to_string(), "3000000001");
//		EXPECT_EQ((first + second).to_string(), "2500000000");

//		//first = "                      3000000001";
//		//LongInteger<3> third("1000000003000000001")

//        uint32_t kek = 1u;
//        auto lol = int32_t(~kek);
//        EXPECT_EQ(lol, -2);
//        int64_t lol2 = int64_t(lol);
//        EXPECT_EQ(lol2, -2);
//        uint64_t lol3 = uint64_t(lol);
//        EXPECT_EQ(lol3, -2);
//	}

}
