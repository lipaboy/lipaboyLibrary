#ifndef ACCURACY_FIXED_NUMBER_H
#define ACCURACY_FIXED_NUMBER_H

#include "../common_interfaces/comparable.h"
#include "../common_interfaces/either_comparable.h"

namespace LipaboyLib {

	template <typename T, typename IntegerType = int>
	inline constexpr T powDozen(IntegerType power) {
		return (power < static_cast<IntegerType>(0)) ? 
				((power > static_cast<IntegerType>(-1)) ? static_cast<T>(1) :
					static_cast<T>(0.1) * powDozen<T>(power + static_cast<IntegerType>(1)))
			: ((power < static_cast<IntegerType>(1)) ? static_cast<T>(1) : static_cast<T>(10)
				* powDozen<T>(power - static_cast<IntegerType>(1)));
	}

	template <typename T, typename IntegerPrecisionType, 
		IntegerPrecisionType fraction, IntegerPrecisionType dozenPower>
	class FixedPrecisionNumber : public Comparable, public EitherComparable<T> {
	public:
		explicit
			FixedPrecisionNumber(T _number = T()) : number(_number) {}

		bool operator<(const T& val) const { return (number < val 
			- static_cast<T>(fraction * powDozen<T, IntegerPrecisionType>(dozenPower))); }
		bool operator<=(const T& val) const { return (number <= val 
			+ static_cast<T>(fraction * powDozen<T, IntegerPrecisionType>(dozenPower))); }
		bool operator==(const T& val) const {
			return (number >= val 
					- static_cast<T>(fraction * powDozen<T, IntegerPrecisionType>(dozenPower)))
				&& (number <= val 
					+ static_cast<T>(fraction * powDozen<T, IntegerPrecisionType>(dozenPower)));
		}

		bool operator<(const Comparable& obj) const { 
			return (*this) < dynamic_cast<const FixedPrecisionNumber&>(obj).number; }
		bool operator<=(const Comparable& obj) const { 
			return (*this) <= dynamic_cast<const FixedPrecisionNumber&>(obj).number; }
		bool operator==(const Comparable& obj) const { 
			return ((*this) == dynamic_cast<const FixedPrecisionNumber&>(obj).number); }

		operator T() { return number; }

	private:
		T number;
	};

}

#endif
