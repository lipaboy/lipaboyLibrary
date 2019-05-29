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

#ifdef BENCHMARK_TEST_RUN
		BigInteger number(2);

		for (int i = 0; i < 10; i++) {
			number *= number;
		}

		cout << bigIntegerToString(number) << endl;
		
		auto start = steady_clock::now();

		for (int i = 3; number > 0; i++) {
			number /= i;
		}
		cout << "Time: " << (steady_clock::now() - start).count() / int(1e6) << endl;


		//system("pause");
		//ASSERT_FALSE(true);
#endif

	}

	TEST(BigInteger, mult_speed) {

#ifdef BENCHMARK_TEST_RUN
		const int N = 24;
		BigUnsigned number1(2);
		BigUnsigned number2(2);

		{
			BigUnsigned & number = number1;
			auto start = steady_clock::now();
			for (int i = 0; i < N; i++) {
				number *= number;
			}
			cout << "Time #1: " << (steady_clock::now() - start).count() / int(1e6) << endl;
		}

		/*{
			BigUnsigned & number = number2;
			auto start = steady_clock::now();
			for (int i = 0; i < N; i++) {
				number.multiplyByKaracuba(number, number);
			}
			cout << "Time #K: " << (steady_clock::now() - start).count() / int(1e6) << endl;
			cout << "Number length = " << number.getLength() << endl;
		}*/

		system("pause");
		ASSERT_TRUE(number1 == number2);
		
		
#endif

	}

}

