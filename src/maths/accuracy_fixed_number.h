#ifndef ACCURACY_FIXED_NUMBER_H
#define ACCURACY_FIXED_NUMBER_H

#include "../common_interfaces/comparable.h"
#include "../common_interfaces/either_comparable.h"

namespace LipaboyLib {

	template <typename T>
	inline constexpr T powDozen(int power) {
		return (power < 0) ? ((power > -1) ? static_cast<T>(1) : static_cast<T>(0.1) * powDozen<T>(power + 1))
			: ((power < 1) ? static_cast<T>(1) : static_cast<T>(10) * powDozen<T>(power - 1));
	}


	template <typename T, int fraction, int dozenPower>
	class AccuracyFixedNumber : public Comparable, public EitherComparable<T> {
	public:
		explicit
			AccuracyFixedNumber(T _number = T()) : number(_number) {}

		bool operator<(const T& val) const { return (number < val - fraction * powDozen<T>(dozenPower)); }
		bool operator<=(const T& val) const { return (number <= val + fraction * powDozen<T>(dozenPower)); }
		bool operator==(const T& val) const {
			return (number >= val - fraction * powDozen<T>(dozenPower))
				&& (number <= val + fraction * powDozen<T>(dozenPower));
		}

		bool operator<(const Comparable& obj) const { return (*this) < dynamic_cast<const AccuracyFixedNumber&>(obj).number; }
		bool operator<=(const Comparable& obj) const { return (*this) <= dynamic_cast<const AccuracyFixedNumber&>(obj).number; }
		bool operator==(const Comparable& obj) const { return ((*this) == dynamic_cast<const AccuracyFixedNumber&>(obj).number); }

		operator T() { return number; }

	private:
		T number;
		//const T precision = fraction * powDozen<T>(dozenPower);
	};

}

#endif
