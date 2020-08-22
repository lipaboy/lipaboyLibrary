#include <gtest/gtest.h>

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <list>
#include <tuple>
#include <functional>
#include <algorithm>

//#include "extra_tools/extra_tools.h"

#include "stream/stream.h"
//#include "extra_tools/extra_tools_tests.h"
//#include "extra_tools/maths_tools.h"
//#include "maths/fixed_precision_number.h"

#include "extra_tools/detect_time_duration.h"

namespace lipaboy_lib_tests {

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::unique_ptr;
using std::any;

using namespace lipaboy_lib;
using namespace lipaboy_lib::stream_space;
using namespace lipaboy_lib::stream_space::operators;


namespace {

	template <class... Args>
	decltype(auto) combine(Args... arg) {
		return std::make_tuple(arg...);
	}

	template <class... Args>
	decltype(auto) combine2(std::list<Args>... list) {
		std::list<std::tuple<Args...> > res;
		size_t const size = std::min({ list.size()... });

		//for (auto & elem : list) 
		//cout << size << endl;

		for (size_t i = 0; i < size; i++) {
			res.push_front(std::tuple<Args...>(list.front()...));
			(list.pop_front(), ...);
		}

		return res;
	}

}

TEST(Check, numberphile) {
    auto startTime = extra::getCurrentTime();

	// initial values are important
    vector< vector<char> > temps(30, vector<char>(50, 1));
    temps[0] = vector<char>(50, 2);
    vector<size_t> sizes(30, 1);
    sizes[0] = 2;

    // (Linux) 1e8 - 12 secs, 1e9 - 116 secs,  1e15 > 5 min
    constexpr long long MAX = static_cast<long long>(1e2);
    size_t maxSteps = 0;
	vector<char> maxElem;
    for (size_t i = 0; i < MAX; i++) {
		
        // multiply the digits until we get the one digit as result

        size_t iTemp = 0;
        for (; ; iTemp++) {
            sizes[iTemp + 1] = 1;
            temps[iTemp + 1][0] = 1;

            for (size_t k = 0; k < sizes[iTemp]; k++) {
				if (temps[iTemp][k] == 1)
					continue;
				// k's digit
                size_t t = 0;
				char remainder = 0; // decade's remainder
				// temps[iTemp + 1] - middle result of multiplication
                for (; t < sizes[iTemp + 1]; t++) {
					char& curr = temps[iTemp + 1][t];
					curr = curr * temps[iTemp][k] + remainder;
					remainder = curr / 10;
                    curr %= 10;
				}
                if (remainder > 0) {
                    sizes[iTemp + 1]++;
                    temps[iTemp + 1][t] = remainder;
                }
			}
            if (sizes[iTemp + 1] <= 1)
                break;
		}

		vector<char>& number = temps[0];

        if (iTemp + 1 >= maxSteps) {
			maxSteps = iTemp + 1;
			maxElem = number;
		}

		// get next number

        size_t j = 0;
        for (; j < sizes[0]; j++) {
			number[j]++;
			if (number[j] < 10)
				break;
            number[j] = 2;
		}
        if (j == sizes[0])
            sizes[0]++;

	}

	cout << "Max steps: " << maxSteps << endl
		<< "Number: ";
    auto vec = Stream(maxElem)
		| map([](char ch) -> int { return int(ch); }) 
        | get(sizes[0])
        | to_vector();
    (Stream(vec.rbegin(), vec.rend()) | print_to(cout)) << endl;

    cout << "Time elapsed: " << extra::diffFromNow(startTime) << endl;

    //string str;
    //std::cin >> str;

	ASSERT_TRUE(false);
}

TEST(Check, numberphile_int64_t) {
    auto startTime = extra::getCurrentTime();

    vector<int64_t> numbers(30, 1);
    numbers[0] = 11;

    // linux: 1e9 - 23 secs, 1e10 - 211 secs
    constexpr uint64_t MAX = static_cast<uint64_t>(1e2);
    uint64_t maxSteps = 0;
    uint64_t maxNumber = 1;
    for (uint64_t i = 0; i < MAX; i++) {

        // multiply the digits

        uint64_t iNum = 0;
        for (; ; iNum++) {
            numbers[iNum + 1] = 1;

            auto curr = numbers[iNum];
            for ( ; curr > 0; ) {
                numbers[iNum + 1] *= curr % 10;
                curr /= 10;
            }

            if (numbers[iNum + 1] < 10)
                break;
        }

        if (maxSteps <= iNum + 1) {
            maxSteps = iNum + 1;
            maxNumber = numbers[0];
        }

        // get next number
        numbers[0]++;
    }

    cout << "Max steps: " << maxSteps << endl
        << "Number: " << maxNumber << endl;

    cout << "Time elapsed: " << extra::diffFromNow(startTime) << endl;

    string str;
    std::cin >> str;

    ASSERT_TRUE(false);
}

TEST(Check, check) {

}

}



int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
	auto res = RUN_ALL_TESTS();
	
    return res;
}
