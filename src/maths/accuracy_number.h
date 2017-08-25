#ifndef ACCURACY_NUMBER_H
#define ACCURACY_NUMBER_H

#include "../common_interfaces/comparable.h"
#include "../common_interfaces/either_comparable.h"

namespace LipaboyLib {
	
	//TODO: think about name, may be PreciselyNumber or UnpreciselyNumber (but ConstUnpreciselyNumber ??)

	//TODO: write Summerizable and so on (like Algebra)

	template <class T>
	class Elemable {
	public:
		Elemable(const T& val) : elem(val) {}

		const Elemable& operator=(const T& val) noexcept { elem = val; return *this; }

		void set(const T& val) { elem = val; }
		const T& get() const { return elem; }
	private:
		T elem;
	};

	////It is extension of Elemable
	//template <class T>
	//struct EitherAssignable : public Elemable<T> {
	//	EitherAssignable(const T& val) : Elemable(val) {}
	//	const T& operator=(const T& val) {
	//		set(val);
	//		return get();
	//	}
	//};

	template <class T>
	class EitherSummable : public Elemable<T> {
	public:
		EitherSummable(const T& val) : Elemable(val) {}
		T operator+(const EitherSummable& other) const { return get() + other.get(); }
	};
	template <class T>
	T operator+(const EitherSummable<T>& obj, const T& val) { return obj.get() + val; }
	template <class T>
	T operator+(const T& val, const EitherSummable<T>& obj) { return val + obj.get(); }

	template <class T>
	class Algebra : public EitherSummable<T> {
	public:
		Algebra(const T& val) : EitherSummable(val) {}
	};
	

	template <class T>
	class AccuracyNumber : public EitherComparable<T>, public Comparable,
					public Algebra<T> {
	public:
		explicit
		AccuracyNumber(const T& _number = T(), const T& _precision = T())
			: Algebra(_number), epsilon(_precision) {}

		bool operator<(const T& val) const { return (get() < val - epsilon); }
		bool operator<=(const T& val) const { return (get() <= val + epsilon); }
		bool operator==(const T& val) const { 
			return (get() >= val - epsilon) && (get() <= val + epsilon);
		}

		bool operator<(const Comparable& obj) const { return (*this) < dynamic_cast<const AccuracyNumber&>(obj).get(); }
		bool operator<=(const Comparable& obj) const { return (*this) <= dynamic_cast<const AccuracyNumber&>(obj).get();}
		bool operator==(const Comparable& obj) const { return ((*this) == dynamic_cast<const AccuracyNumber&>(obj).get()); }

		operator T() { return get(); }
		//const AccuracyNumber&

	private:
		//TODO: you can make number public for summarizing
		//T number;
		T epsilon;		//our precision
	};

	typedef AccuracyNumber<double> AccuracyDouble;

}



//template <double _precision>
//using ConstAccuracyDouble = AccuracyFixedNumber<double, _precision>;

#endif	//ACCURACY_NUMBER_H
