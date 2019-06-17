#include <gtest/gtest.h>
#include <cmath>
#include <valarray>
#include <iostream>
#include <time.h>
#include <chrono>
#include <vector>

#include "long_numbers/big_integer_library.h"

//#define BENCHMARK_TEST_RUN

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


	using std::vector;
	using std::istream;
	// current base functions use for operate with long integers
	constexpr int base = 1e7;
	// lenght of the long number for which naive multiplication
	// will be called in the Karatsuba function
	constexpr unsigned int len_f_naive = 32;
	// One digit size for numbers with bases multiple of ten
	constexpr int dig_size = 10;
	// How much zeroes have to be in the number
	constexpr int add_zero = base / dig_size;

	vector<int> get_number(std::string snum) {
		vector<int> vnum;
		// индикатор разрядов
		unsigned int dig = 1;
		int n = 0;


		for (auto it = snum.crbegin(); it != snum.crend(); ++it) {
			n += (*it - '0') * dig;
			dig *= dig_size;
			// если разряд равен базе, то выталкиваем число в вектор
			if (dig == base) {
				vnum.push_back(n);
				n = 0;
				dig = 1;
			}
		}

		if (n != 0) {
			vnum.push_back(n);
		}

		return vnum;
	}

	void extend_vec(vector<int>& v, size_t len) {
		while (len & (len - 1)) {
			++len;
		}

		v.resize(len);
	}

	vector<int> naive_mul(const vector<int>& x, const vector<int>& y) {
		auto len = x.size();
		vector<int> res(2 * len);

		for (auto i = 0; i < len; ++i) {
			for (auto j = 0; j < len; ++j) {
				res[i + j] += x[i] * y[j];
			}
		}

		return res;
	}

	vector<int> karatsuba_mul(const vector<int>& x, const vector<int>& y) {
		auto len = x.size();
		vector<int> res(2 * len);

		if (len <= len_f_naive) {
			return naive_mul(x, y);
		}

		auto k = len / 2;

		vector<int> Xr{ x.begin(), x.begin() + k };
		vector<int> Xl{ x.begin() + k, x.end() };
		vector<int> Yr{ y.begin(), y.begin() + k };
		vector<int> Yl{ y.begin() + k, y.end() };

		vector<int> P1 = karatsuba_mul(Xl, Yl);
		vector<int> P2 = karatsuba_mul(Xr, Yr);

		vector<int> Xlr(k);
		vector<int> Ylr(k);

		for (int i = 0; i < k; ++i) {
			Xlr[i] = Xl[i] + Xr[i];
			Ylr[i] = Yl[i] + Yr[i];
		}

		vector<int> P3 = karatsuba_mul(Xlr, Ylr);

		for (auto i = 0; i < len; ++i) {
			P3[i] -= P2[i] + P1[i];
		}

		for (auto i = 0; i < len; ++i) {
			res[i] = P2[i];
		}

		for (auto i = len; i < 2 * len; ++i) {
			res[i] = P1[i - len];
		}

		for (auto i = k; i < len + k; ++i) {
			res[i] += P3[i - k];
		}

		return res;
	}

	void finalize(vector<int>& res) {
		for (auto i = 0; i < res.size() - 1; ++i) {
			res[i + 1] += res[i] / base;
			res[i] %= base;
		}
	}
	



	TEST(BigUnsigned, mult_speed) {

#ifdef BENCHMARK_TEST_RUN
		const int N = 17;
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
			for (int i = 0; i < N - 1; i++) {
				number = multiplyByKaracuba(number, number);
			}
			number = multiplyByKaracuba(number, number);
			cout << "Time #K: " << (steady_clock::now() - start).count() / int(1e6) << endl;
			cout << "Number length = " << number.length() << endl;
		}*/

		{
			std::vector<int> number = get_number("2");
			auto start = steady_clock::now();
			for (int i = 0; i < N - 1; i++) {
				number = karatsuba_mul(number, number);
				finalize(number);
			}
			number = karatsuba_mul(number, number);
			finalize(number);
			cout << "Time #K_2: " << (steady_clock::now() - start).count() / int(1e6) << endl;
			int len;
			for (len = number.size() - 1; len >= 0; len--) {
				if (number[len] != 0)
					break;
			}
			number.resize(len + 1);
			cout << "Number length = " << number.size() << endl;

		}

		system("pause");
		ASSERT_TRUE(number1 == number2);
		
		
#endif

	}

}

