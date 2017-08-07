#ifndef ACCURACY_NUMBER_H
#define ACCURACY_NUMBER_H

#include "../common/comparable.h"
#include "../common/either_comparable.h"

namespace LipaboyLib {

	//TODO: think about name, may be PreciselyNumber or UnpreciselyNumber (but ConstUnpreciselyNumber ??)

	//TODO: write Summerizable and so on (like Algebra)

	template <class T>
	class AccuracyNumber : public EitherComparable<T>, public Comparable {
	public:
		explicit
		AccuracyNumber(T _number = T(), T _precision = T())
			: number(_number), epsilon(_precision) {}

		virtual bool operator<(const T& val) const { return (number < val - epsilon); }
		virtual bool operator<=(const T& val) const { return (number <= val + epsilon); }
		virtual bool operator==(const T& val) const { 
			return (number >= val - epsilon) && (number <= val + epsilon); 
		}

		virtual bool operator<(const Comparable& obj) const { return (*this) < dynamic_cast<const AccuracyNumber&>(obj).number; }
		virtual bool operator<=(const Comparable& obj) const { return (*this) <= dynamic_cast<const AccuracyNumber&>(obj).number;}
		virtual bool operator==(const Comparable& obj) const { return ((*this) == dynamic_cast<const AccuracyNumber&>(obj).number); }

		operator T() { return number; }
		//const AccuracyNumber&

	private:
		T number;
		T epsilon;		//our precision
	};

	typedef AccuracyNumber<double> AccuracyDouble;

}

template <typename T>
inline constexpr T powDozen(int power) {
	return (power < 0) ? ((power > -1) ? static_cast<T>(1) : static_cast<T>(0.1) * powDozen<T>(power + 1))
		: ((power < 1) ? static_cast<T>(1) : static_cast<T>(10) * powDozen<T>(power - 1));
}

/*------------Too a lot of code production (but more faster)------------*/

template <typename T, int fraction, int dozenPower>
class ConstAccuracyNumber {
public:
	explicit
	ConstAccuracyNumber(T _number = static_cast<T>(0)) : number(_number) {}

	bool operator<(const T& val) const { return (number < val - fraction * powDozen<T>(dozenPower)); }
	bool operator>(const T& val) const { return (number > val + fraction * powDozen<T>(dozenPower)); }
	bool operator<=(const T& val) const { return !((*this) > val); }
	bool operator>=(const T& val) const { return !((*this) < val); }
	bool operator==(const T& val) const { return ((*this) >= val) && ((*this) <= val); }
	bool operator!=(const T& val) const { return !((*this) == val); }

	bool operator<(const ConstAccuracyNumber& obj) const { return number < obj.number; }
	bool operator>(const ConstAccuracyNumber& obj) const { return number > obj.number; }
	bool operator<=(const ConstAccuracyNumber& obj) const { return !((*this) > obj); }
	bool operator>=(const ConstAccuracyNumber& obj) const { return !((*this) < obj); }
	bool operator==(const ConstAccuracyNumber& obj) const { return ((*this) >= obj) && ((*this) <= obj); }
	bool operator!=(const ConstAccuracyNumber& obj) const { return !((*this) == obj); }

	operator T() { return number; }

	//const AccuracyNumber


private:
	T number;
	//const T precision = fraction * powDozen<T>(dozenPower);
};

//template <double _precision>
//using ConstAccuracyDouble = ConstAccuracyNumber<double, _precision>;

#endif	//ACCURACY_NUMBER_H
