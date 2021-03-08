#include <gtest/gtest.h>
#include <cmath>
#include <valarray>
#include <iostream>
#include <time.h>
#include <chrono>
#include <vector>

#include "long_numbers/long_integer_decimal.h"

#include "extra_tools/detect_time_duration.h"

namespace long_numbers_benchmark_tests_space {

	using std::cout;
	using std::endl;
	using namespace std::chrono;
	using namespace lipaboy_lib::long_numbers_space;
    using namespace lipaboy_lib::extra;

    // Results:
    // Linux <Mult count>
    // <158> LongIntegerDecimal<8> - 0.231 s, BigUnsigned - 16.2 s
    // <158> LongIntegerDecimal<16> - 0.306 s
    TEST(LongNumberDecimal, DISABLED_benchmark) {
        constexpr int ROUND_TRIP = 5000//100000
			;
        constexpr int MULT_COUNT = 38 //+ 40 + 80
			;
        using LongI = LongIntegerDecimal<4//16
		>;

        uint64_t num = 3;   // 3^40
        auto start1 = getCurrentTime();
        // 1e5 - 5 secs for LongNumber
        for (int round = 0; round < ROUND_TRIP; round++) {
            for (int i = 0; i < MULT_COUNT; i++) {
                num *= 3;
            }
            num = 3;
        }
        cout << "Time 1: " << diffFromNow(start1) << endl;

        LongI l("3");
        auto start2 = getCurrentTime();
        for (int round = 0; round < ROUND_TRIP; round++) {
            for (int i = 0; i < MULT_COUNT; i++) {
                l = l * LongI(3);
            }
            l = LongI(3);
        }

        cout << "Time 3: " << diffFromNow(start2) << endl;

		LongI lk("3");
		auto start4 = getCurrentTime();
		for (int round = 0; round < ROUND_TRIP; round++) {
			for (int i = 0; i < MULT_COUNT; i++) {
				lk = lk.multiplyByKaracuba( LongI(3) );
			}
			lk = LongI(3);
		}

		cout << "Time 4: " << diffFromNow(start4) << endl;

        string s;
        std::cin >> s;
    }


	using std::vector;
	using std::istream;
	// current base functions use for operate with long integers
	const int base = int(1e7);
	// lenght of the long number for which naive multiplication
	// will be called in the Karatsuba function
	const unsigned int len_f_naive = 32;
	// One digit size for numbers with bases multiple of ten
	const int dig_size = 10;
	// How much zeroes have to be in the number
	const int add_zero = base / dig_size;

	vector<int> get_number(std::string snum) {
		vector<int> vnum;
        // indicator of digits
		unsigned int dig = 1;
		int n = 0;


		for (auto it = snum.crbegin(); it != snum.crend(); ++it) {
			n += (*it - '0') * dig;
			dig *= dig_size;
            // if indicator == base then push the number into vector
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

		for (size_t i = 0; i < len; ++i) {
			for (size_t j = 0; j < len; ++j) {
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

		size_t k = len / 2;

		vector<int> Xr{ x.begin(), x.begin() + k };
		vector<int> Xl{ x.begin() + k, x.end() };
		vector<int> Yr{ y.begin(), y.begin() + k };
		vector<int> Yl{ y.begin() + k, y.end() };

		vector<int> P1 = karatsuba_mul(Xl, Yl);
		vector<int> P2 = karatsuba_mul(Xr, Yr);

		vector<int> Xlr(k);
		vector<int> Ylr(k);

		for (size_t i = 0; i < k; ++i) {
			Xlr[i] = Xl[i] + Xr[i];
			Ylr[i] = Yl[i] + Yr[i];
		}

		vector<int> P3 = karatsuba_mul(Xlr, Ylr);

		for (size_t i = 0; i < len; ++i) {
			P3[i] -= P2[i] + P1[i];
		}

		for (size_t i = 0; i < len; ++i) {
			res[i] = P2[i];
		}

		for (size_t i = len; i < 2 * len; ++i) {
			res[i] = P1[i - len];
		}

		for (size_t i = k; i < len + k; ++i) {
			res[i] += P3[i - k];
		}

		return res;
	}

	void finalize(vector<int>& res) {
		for (size_t i = 0; i < res.size() - 1; ++i) {
			res[i + 1] += res[i] / base;
			res[i] %= base;
		}
	}

}

