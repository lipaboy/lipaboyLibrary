#include "maths/fixed_precision_number.h"

#include <gtest/gtest.h>
#include <string>
#include <iostream>

namespace lipaboy_lib_tests {

	using lipaboy_lib::FixedPrecisionNumber;

	TEST(FixedPrecisionNumber, simple) {
		FixedPrecisionNumber<double, 1, -5> kek(5.);

		ASSERT_EQ(kek, 5.);
		ASSERT_EQ(5., kek);
		ASSERT_NE(5.2, kek);
		ASSERT_NE(5.0002, kek);
		ASSERT_NE(5.00002, kek);
		ASSERT_EQ(5.000002, kek);
	}

	TEST(FixedPrecisionNumber, initializing) {
		FixedPrecisionNumber<double, 1, -5> kek = 2.;
		ASSERT_EQ(kek, 2.);
	}

}

