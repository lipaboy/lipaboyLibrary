#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <time.h>
#include <chrono>
#include <vector>

#include "long_numbers/long_integer.h"
#include "long_numbers/long_unsigned.h"

#include "extra_tools/detect_time_duration.h"

namespace long_numbers_benchmark_tests_space {

    using std::cout;
    using std::endl;
    using namespace std::chrono;
    using namespace lipaboy_lib::long_numbers_space;
    using namespace lipaboy_lib::extra;

    // Results:
    // Results: (Windows, AMD Ryzen 5 3500U 2019 year old, Release, 5e6)
    // uint64_t - 37 ms, LongInteger<2> - 2152 ms, LongUnsigned<2> - 2027 ms
    // Results: (Windows, AMD Ryzen 5 3500U 2019 year old, Release, 5e7)
    // uint64_t - 361 ms, LongInteger<2> - 21492 ms, LongUnsigned<2> - 18539 ms
    TEST(LongIntegerVsLongUnsigned, DISABLED_benchmark) {
        constexpr int ROUND_TRIP = int(5e6) //100000
            ;
        constexpr int MULT_COUNT = 38
            ;
        using LongI = LongInteger<2>;
        using LongU = LongUnsigned<2>;

        uint64_t num = 3u;   // 3^40
        auto start1 = getCurrentTime();
        // 1e5 - 5 secs for LongNumber
        for (int round = 0; round < ROUND_TRIP; round++) {
            num = 3u;
            for (int i = 0; i < MULT_COUNT; i++) {
                num = num * 3u;
            }
        }
        cout << "Time uint64_t: " << diffFromNow(start1) << endl;

        LongI l(3);
        auto start2 = getCurrentTime();
        for (int round = 0; round < ROUND_TRIP; round++) {
            l = LongI(3);
            for (int i = 0; i < MULT_COUNT; i++) {
                l = l * LongI(3);
            }
        }

        cout << "Time LongInteger: " << diffFromNow(start2) << endl;

        LongU lu(3);
        auto start4 = getCurrentTime();
        for (int round = 0; round < ROUND_TRIP; round++) {
            lu = LongU(3);
            for (int i = 0; i < MULT_COUNT; i++) {
                lu = lu * LongU(3);
            }
        }

        cout << "Time LongUnsigned: " << diffFromNow(start4) << endl;

        EXPECT_EQ(std::to_string(num), l.to_string());
        EXPECT_EQ(l.to_string(), lu.to_string());

        std::string s;
        std::cin >> s;
    }

}

