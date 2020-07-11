#include "gtest/gtest.h"

#include <iostream>

#include "common_interfaces/algebra.h"
#include "maths/fixed_precision_number.h"

namespace lipaboy_lib_tests {

	using std::cout;
	using std::endl;

	using lipaboy_lib::FixedPrecisionNumber;


	TEST(Algebra, check) {
		FixedPrecisionNumber<double, 1, -5>
			kek1(2),
			kek2(3);
		FixedPrecisionNumber<double, 1, -8>
			pup(-4);

		ASSERT_EQ(kek1 + kek2, 5);
		ASSERT_EQ(kek1 + 4., 6.);
		ASSERT_EQ((kek1 + kek2) / (kek1 - kek2), -5);
		ASSERT_EQ(kek1 + pup, -2);
		ASSERT_EQ(pup + kek1, -2);

		ASSERT_EQ(kek1 - kek2, -1.);
		ASSERT_EQ(kek1 - 4., -2.);
		ASSERT_EQ(kek1 - pup, 6.);

		ASSERT_EQ(kek1 * kek2, 6.);
		ASSERT_EQ(kek1 * 4., 8.);
		ASSERT_EQ(kek1 * pup, -8.);

		ASSERT_EQ(kek1 / kek2, 2. / 3.);
		ASSERT_EQ(kek1 / 4., 1. / 2.);
		ASSERT_EQ(kek1 / pup, -1. / 2.);
	}

	TEST(NumberSelfSummable, fixed_precision_number) {
		FixedPrecisionNumber<double, 1, -5> kek1(2.);
		FixedPrecisionNumber<double, 1, -7> kek2(2.f);

		ASSERT_EQ(kek1 += 2., 4.);
		ASSERT_EQ((kek1 += FixedPrecisionNumber<double, 1, -5>(2.)), 6.);
		ASSERT_EQ(kek1 += kek2, 8.);

		kek1 = 2.;
		ASSERT_EQ(kek1 -= 2., 0.);
		ASSERT_EQ((kek1 -= FixedPrecisionNumber<double, 1, -5>(2.)), -2.);
		ASSERT_EQ(kek1 -= kek2, -4.);

		kek1 = 2.;
		ASSERT_EQ(kek1 *= 3., 6.);
		ASSERT_EQ((kek1 *= FixedPrecisionNumber<double, 1, -5>(2.)), 12.);
		ASSERT_EQ(kek1 *= kek2, 24.);

		ASSERT_EQ(kek1 /= 3., 8.);
		ASSERT_EQ((kek1 /= FixedPrecisionNumber<double, 1, -5>(2.)), 4.);
		ASSERT_EQ(kek1 /= kek2, 2.);
	}

	TEST(EitherComparable, comparison) {
		FixedPrecisionNumber<double, 1, -5>
			kek1(2),
			kek2(2.00001);
		FixedPrecisionNumber<double, 1, -8>
			pup(-4);
		// not work (maybe on linux?)
		ASSERT_NE(kek1, 2.00002);
		ASSERT_EQ(kek1, 2.00001);

		ASSERT_EQ(kek1, kek2);
		ASSERT_TRUE(kek1 == kek2);
		kek2 += 2.;
		ASSERT_NE(kek1, kek2);
		ASSERT_TRUE(kek1 <= kek2);

		ASSERT_TRUE(kek1 >= 2.);

		using ConstFixedDouble3 = const FixedPrecisionNumber<double, 1, -3>;
		ASSERT_TRUE(ConstFixedDouble3(5.) >= ConstFixedDouble3(5.0001));

		ASSERT_TRUE(2.00002 > kek1);
		ASSERT_TRUE(2.00002 >= kek1);
		ASSERT_FALSE(2.00001 > kek1);
		ASSERT_TRUE(2.00001 >= kek1);
		ASSERT_TRUE(2.00001 <= kek1);
		ASSERT_FALSE(2.00001 < kek1);
	}

}