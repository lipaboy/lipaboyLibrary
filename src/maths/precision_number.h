#ifndef ACCURACY_NUMBER_H
#define ACCURACY_NUMBER_H

#include "../common_interfaces/comparator_extender.h"
#include "../common_interfaces/another_type_comparing_extender.h"
#include "../common_interfaces/algebra.h"

#include <ostream>

namespace lipaboy_lib {

    // PLAN
    //---------
    // TODO: add set epsilon
    // TODO: measure the time of assessing element, of summarizing, algebra interfaces

	template <class T>
	class PrecisionNumber : 
		//public EitherComparable<T>, 
        public ComparatorExtender<PrecisionNumber<T> >,
        public Algebra<T, PrecisionNumber<T> >,
		public NumberSettable<T>
	{
    public:
        using ValueType = T;
	public:
		explicit
		PrecisionNumber(const T& _number = T(), const T& _precision = T()) noexcept
			: number(_number), epsilon(_precision) {}

        bool operator<(const T& val) const noexcept { return (getNumber() < val - epsilon); }
		bool operator==(const T& val) const noexcept { 
            return (getNumber() >= val - epsilon) && (getNumber() <= val + epsilon);
		}

        bool operator<(const PrecisionNumber& obj) const noexcept {
            return (*this) < obj.getNumber(); }
        bool operator==(const PrecisionNumber& obj) const noexcept {
            return (*this) == obj.getNumber(); }

		void setNumber(T const & val) noexcept { number = val; }
        T const & getNumber() const noexcept { return number; }
		PrecisionNumber const & operator= (T const & val) noexcept { setNumber(val); return *this; }

//        operator T() noexcept { return getNumber(); }

		//friend std::ostream& operator<< (std::ostream& o, PrecisionNumber<T> const & number);
	private:
		T epsilon;		//our precision
		T number;
	};

	/*template <class T>
	inline std::ostream& operator<< (std::ostream& o, PrecisionNumber<T> const & number) { 
		return o << number.getNumber(); 
	}*/

	typedef PrecisionNumber<double> PrecisionDouble;

}



//template <double _precision>
//using ConstAccuracyDouble = AccuracyFixedNumber<double, _precision>;

#endif	//ACCURACY_NUMBER_H
