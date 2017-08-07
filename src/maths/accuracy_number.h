#ifndef ACCURACY_NUMBER_H
#define ACCURACY_NUMBER_H

#include "../common_interfaces/comparable.h"
#include "../common_interfaces/either_comparable.h"

namespace LipaboyLib {
	
	//TODO: think about name, may be PreciselyNumber or UnpreciselyNumber (but ConstUnpreciselyNumber ??)

	//TODO: write Summerizable and so on (like Algebra)

	template <class T>
	class AccuracyNumber : public EitherComparable<T>, public Comparable {
	public:
		explicit
		AccuracyNumber(T _number = T(), T _precision = T())
			: number(_number), epsilon(_precision) {}

		bool operator<(const T& val) const { return (number < val - epsilon); }
		bool operator<=(const T& val) const { return (number <= val + epsilon); }
		bool operator==(const T& val) const { 
			return (number >= val - epsilon) && (number <= val + epsilon); 
		}

		bool operator<(const Comparable& obj) const { return (*this) < dynamic_cast<const AccuracyNumber&>(obj).number; }
		bool operator<=(const Comparable& obj) const { return (*this) <= dynamic_cast<const AccuracyNumber&>(obj).number;}
		bool operator==(const Comparable& obj) const { return ((*this) == dynamic_cast<const AccuracyNumber&>(obj).number); }

		operator T() { return number; }
		//const AccuracyNumber&

	private:
		T number;
		T epsilon;		//our precision
	};

	typedef AccuracyNumber<double> AccuracyDouble;

}



//template <double _precision>
//using ConstAccuracyDouble = AccuracyFixedNumber<double, _precision>;

#endif	//ACCURACY_NUMBER_H
