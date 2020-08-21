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
	vector< vector<char> > temps = { {1, 1}, {1} };

    constexpr long long MAX = 10000;
    size_t maxSteps = 0;
	vector<char> maxElem;
	for (int i = 0; i < MAX; i++) {
		
		// multiply the digits until we get the one digit as result

		if (i == 14) {
			cout << "opa";
		}

		bool isZeroFound = false;
        size_t iTemp = 0;
		for (; ; iTemp++) {
			if (iTemp >= temps.size() - 1)
				temps.push_back({ 1 });
			else {
				temps[iTemp + 1].resize(1);
				temps[iTemp + 1][0] = 1;
			}
            for (size_t k = 0; k < temps[iTemp].size(); k++) {
				if (temps[iTemp][k] == 1)
					continue;
				// k's digit
                size_t t = 0;
				char remainder = 0; // decade's remainder
				// temps[iTemp + 1] - middle result of multiplication
				for (; t < temps[iTemp + 1].size(); t++) {
					char& curr = temps[iTemp + 1][t];
					curr = curr * temps[iTemp][k] + remainder;
					remainder = curr / 10;
					curr %= 10;
					if (curr == 0) {
						isZeroFound = true;
						break;
					}
				}
				if (t == temps[iTemp + 1].size() && remainder > 0) {
					temps[iTemp + 1].push_back(remainder);
				}
				if (isZeroFound)
					break;
			}
			if (temps[iTemp + 1].size() <= 1)
				break;
			if (isZeroFound)
				break;
		}

		vector<char>& number = temps[0];

		if (iTemp + 1 > maxSteps) {
			maxSteps = iTemp + 1;
			maxElem = number;
		}

		// get next number

        size_t j = 0;
		for (; j < number.size(); j++) {
			number[j]++;
			if (number[j] < 10)
				break;
			number[j] = 1;
		}
		if (j == number.size())
			number.push_back(1);

	}

	cout << "Max steps: " << maxSteps << endl
		<< "Number: ";
	(Stream(maxElem) 
		| map([](char ch) -> int { return int(ch); }) 
		| print_to(cout)) << endl;

    cout << "Time elapsed: " << extra::diffFromNow(startTime) << endl;

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
