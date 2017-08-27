#ifndef ACCURACY_NUMBER_H
#define ACCURACY_NUMBER_H

#include "../common_interfaces/comparable.h"
#include "../common_interfaces/either_comparable.h"
#include "../common_interfaces/algebra.h"

#include <ostream>

namespace LipaboyLib {
	
	//TODO: think about name, may be PreciselyNumber or UnpreciselyNumber (but ConstUnpreciselyNumber ??)

	//TODO: write Summerizable and so on (like Algebra)

	

	template <class T>
	class AccuracyNumber : public EitherComparable<T>, public Comparable,
					public Algebra<T> {
	public:
		explicit
		AccuracyNumber(const T& _number = T(), const T& _precision = T()) noexcept
			: Algebra(_number), epsilon(_precision) {}

		bool operator<(const T& val) const noexcept { return (get() < val - epsilon); }
		bool operator<=(const T& val) const noexcept { return (get() <= val + epsilon); }
		bool operator==(const T& val) const noexcept { 
			return (get() >= val - epsilon) && (get() <= val + epsilon);
		}

		bool operator<(const Comparable& obj) const noexcept { return (*this) < dynamic_cast<const AccuracyNumber&>(obj).get(); }
		bool operator<=(const Comparable& obj) const noexcept { return (*this) <= dynamic_cast<const AccuracyNumber&>(obj).get();}
		bool operator==(const Comparable& obj) const noexcept { return ((*this) == dynamic_cast<const AccuracyNumber&>(obj).get()); }

		AccuracyNumber const & operator= (T const & val) noexcept { Elemable<T>::operator=(val); return *this; }

		operator T() noexcept { return get(); }

		friend std::ostream& operator<< (std::ostream& o, AccuracyNumber const & number);

	private:
		T epsilon;		//our precision
	};

	template <class T>
	inline std::ostream& operator<< (std::ostream& o, AccuracyNumber<T> const & number) { return o << number.get(); }

	typedef AccuracyNumber<double> AccuracyDouble;

}



//template <double _precision>
//using ConstAccuracyDouble = AccuracyFixedNumber<double, _precision>;

#endif	//ACCURACY_NUMBER_H
