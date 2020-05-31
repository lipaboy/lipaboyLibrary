#include "extra_tools/maths_tools.h"
#include "maths/fixed_precision_number.h"

#include <gtest/gtest.h>
#include <string>
#include <iostream>

namespace lipaboy_lib_tests {

	using lipaboy_lib::powDozen;
	using lipaboy_lib::FixedIntPrecisionNumber;
	
	TEST(Maths_Tools, powDozen) {
		ASSERT_EQ(powDozen<int>(1), 10);
		ASSERT_EQ(powDozen<int>(2), 100);
		ASSERT_EQ(powDozen<uint32_t>(1), 10u);
		ASSERT_EQ(powDozen<uint32_t>(3), 1000u);
		ASSERT_EQ(powDozen<uint32_t>(9), 1000000000u);
		ASSERT_TRUE((powDozen<double>(-2) == FixedIntPrecisionNumber<double, 1, -8>(1e-2)));
		ASSERT_TRUE((powDozen<double>(-10) == FixedIntPrecisionNumber<double, 1, -12>(1e-10)));
	}

}

