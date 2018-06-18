#include "gtest/gtest.h"

#include <iostream>
#include <vector>
#include <string>

#include "common_interfaces/algebra.h"
#include "maths/fixed_precision_number.h"
#include "intervals/interval.h"

#include "stream/stream_test.h"

#include "hash_map/hash_map_test.h"

namespace check_tests {

using std::cout;
using std::endl;
using std::vector;
using std::string;

using LipaboyLib::FixedPrecisionNumber;
using LipaboyLib::Interval;


TEST(Algebra, check) {
    FixedPrecisionNumber<double, int, 1, -5>
            kek1(2),
            kek2(3);
    FixedPrecisionNumber<double, int, 1, -8>
            kuk(-4);

    ASSERT_EQ(kek1 + kek2, 5);
    ASSERT_EQ(kek1 + 4., 6.);
    ASSERT_EQ((kek1 + kek2) / (kek1 - kek2), -5);
    ASSERT_EQ(kek1 + kuk, -2);

    ASSERT_EQ(kek1 - kek2, -1.);
    ASSERT_EQ(kek1 - 4., -2.);
    ASSERT_EQ(kek1 - kuk, 6.);

    ASSERT_EQ(kek1 * kek2, 6.);
    ASSERT_EQ(kek1 * 4., 8.);
    ASSERT_EQ(kek1 * kuk, -8.);

    ASSERT_EQ(kek1 / kek2, 2. / 3.);
    ASSERT_EQ(kek1 / 4.,   1. / 2.);
    ASSERT_EQ(kek1 / kuk, -1. / 2.);
}

TEST(EitherComparable, comparison) {
    FixedPrecisionNumber<double, int, 1, -5>
            kek1(2);
    // not work
//    ASSERT_NE(kek1, 2.00002);
    ASSERT_EQ(kek1, 2.00001);
}

TEST(Interval, contains) {
    Interval<int, std::less<>, std::less<> > interval(1, 5);
    ASSERT_TRUE(interval.containsAll(2, 3, 4));
    ASSERT_TRUE(interval.containsNone(0, 1, 5));
}


}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
