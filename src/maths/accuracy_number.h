#ifndef ACCURACY_NUMBER_H
#define ACCURACY_NUMBER_H

namespace LipaboyMaths {

	//TODO: think about name, may be PreciselyNumber or UnpreciselyNumber (but ConstUnpreciselyNumber ??)

	template <class T>
	class AccuracyNumber {
	public:
		AccuracyNumber(T _number = static_cast<T>(0), T _precision = static_cast<T>(0))
			: number(_number), epsilon(_precision) {}

		bool operator<(const T& val) const { return (number < val - epsilon); }
		bool operator>(const T& val) const { return (number > val + epsilon); }
		bool operator<=(const T& val) const { return !((*this) > val); }
		bool operator>=(const T& val) const { return !((*this) < val); }
		bool operator==(const T& val) const { return ((*this) >= val) && ((*this) <= val); }
		bool operator!=(const T& val) const { return !((*this) == val); }

		bool operator<(const AccuracyNumber& obj) const { return number < obj.number; }
		bool operator>(const AccuracyNumber& obj) const { return number > obj.number; }
		bool operator<=(const AccuracyNumber& obj) const { return !((*this) > obj); }
		bool operator>=(const AccuracyNumber& obj) const { return !((*this) < obj); }
		bool operator==(const AccuracyNumber& obj) const { return ((*this) >= obj) && ((*this) <= obj); }
		bool operator!=(const AccuracyNumber& obj) const { return !((*this) == obj); }

		operator T() { return number; }
		//const AccuracyNumber&

	private:
		T number;
		T epsilon;		//our precision
	};

	typedef AccuracyNumber<double> AccuracyDouble;

}

/*------------Too a lot of code production (but more faster)------------*/

template <typename T, typename PrecisionType, PrecisionType fraction, PrecisionType dozenExponent>
class ConstAccuracyNumber {
public:
	ConstAccuracyNumber(T _number = static_cast<T>(0)) : number(_number) {}

	bool operator<(const T& val) const { return (number < val - precision); }
	bool operator>(const T& val) const { return (number > val + precision); }
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
	//constexpr const T precision = fraction * std::pow(10, dozenExponent);
	 const T precision = 1e5;
};

//template <double _precision>
//using ConstAccuracyDouble = ConstAccuracyNumber<double, _precision>;

#endif	//ACCURACY_NUMBER_H
