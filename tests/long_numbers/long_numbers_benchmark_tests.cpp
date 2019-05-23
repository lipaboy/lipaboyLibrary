#include <gtest/gtest.h>
#include <cmath>
#include <valarray>
#include <iostream>
#include <time.h>
#include <chrono>

#include "long_numbers/big_integer_library.h"

#define BENCHMARK_TEST_RUN

namespace long_numbers_benchmark_tests_space {

	using std::cout;
	using std::endl;
	using namespace std::chrono;
	using namespace lipaboy_lib::long_numbers_space;

	TEST(BigInteger, division_speed) {
		BigInteger number(2);

		for (int i = 0; i < 13; i++) {
			number *= number;
		}

		cout << bigIntegerToString(number) << endl;
		
		auto start = steady_clock::now();

		for (int i = 3; number > 0; i++) {
			number /= i;
		}
		cout << "Time: " << (steady_clock::now() - start).count() / int(1e6) << endl;

#ifdef BENCHMARK_TEST_RUN
		system("pause");
		ASSERT_FALSE(true);
#endif

	}

}

