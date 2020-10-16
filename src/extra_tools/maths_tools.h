#pragma once

// debugging
#include <iostream>
#include <string>

namespace lipaboy_lib {

	using std::cout;
	using std::endl;



	// TODO: write tests
	template <typename T, typename IntegerType = int>
	inline constexpr T powDozen(IntegerType power) {
		return (power < IntegerType(0)) ?
			((power > IntegerType(-1)) ? T(1) :
				T(0.1) * powDozen<T>(power + IntegerType(1)))
			: ((power < IntegerType(1)) ? T(1) : T(10)
				* powDozen<T>(power - IntegerType(1)));
	}

	namespace special {

		template <typename T, T value, typename IntegerType = int>
		inline constexpr T pow(IntegerType power) {
			return (power < IntegerType(0)) ?
				((power > IntegerType(-1)) ? T(1) :
					T(1. / value) * pow<T, value>(power + IntegerType(1)))
				: ((power < IntegerType(1)) ? T(1) : T(value)
					* pow<T, value>(power - IntegerType(1)));
		}

	}

	namespace base {

		// Motivation:
		// no in MCVS Compiler - https://en.cppreference.com/w/cpp/utility/to_chars
		// https://stackoverflow.com/questions/20427596/c-easy-way-to-convert-int-to-string-with-unknown-base
		// https://stackoverflow.com/questions/3799595/itoa-function-problem
		// to difficult - https://en.cppreference.com/w/cpp/string/byte/strtol

		using StringType = std::string;

		inline StringType intToChars(uint32_t number, unsigned int base) {
			StringType res;
			int i = 0;
			while (number > 0) {
				uint32_t digit = number % base;
				res.push_back(digit + '0');
				number /= base;
			}
			return std::string(res.rbegin(), res.rend());
		}

	}

}
