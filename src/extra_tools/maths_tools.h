#pragma once

// debugging
#include <iostream>

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

}
