#include "gtest/gtest.h"

#include <iostream>

#include "intervals/interval.h"
#include "intervals/numerical_interval.h"

namespace lipaboy_lib_tests {

	using std::cout;
	using std::endl;

	using namespace lipaboy_lib;

	TEST(Interval, contains) {
		OpenInterval<int> interval(1, 5);
		ASSERT_TRUE(interval.containsAll(2, 3, 4));
		ASSERT_TRUE(interval.containsNone(0, 1, 5));
	}

	TEST(Ray, simple) {
		PositiveRay<int, std::less<> > rayPos(5);
		ASSERT_TRUE(rayPos.containsNone(0, 1, 5));
		ASSERT_TRUE(rayPos.containsAll(6, 7, 8, 9, INT_MAX));

		NegativeRay<int, std::less<> > rayNeg(5);
		ASSERT_TRUE(rayNeg.containsNone(5, 6, 7, INT_MAX));
		ASSERT_TRUE(rayNeg.containsAll(-2, 0, 1, 4, INT_MIN));

		//ASSERT_TRUE(false);
	}

	TEST(Universum, simple) {
		UniversumInterval<int> universum;

		ASSERT_TRUE(universum.containsAll(-3, 5, 0, INT_MAX, INT_MIN, 1, -1));
	}

	TEST(NumericalInterval, length) {
		NumericalInterval<int, std::less<>, std::less_equal<> > interval(-5, 5);

		ASSERT_EQ(interval.length(), 10);

		CloseNumericalInterval<int> interval2(5, 5);

		ASSERT_EQ(interval2.length(), 0);
	}

}