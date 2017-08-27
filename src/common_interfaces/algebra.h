#ifndef ALGEBRA_H
#define ALGEBRA_H

#include <ostream>

namespace LipaboyLib {

	template <class T>
	class NumberGettable { 
	public: 
		virtual T const& getNumber() const noexcept = 0; 
	};
	//TODO: READ ABOUT THE TENSION BETWEEN OOP AND GENERIC PROGRAMMING
	//I don't know why it doesn't work
	/*template <class T>
	inline std::ostream& operator<< (std::ostream& o, NumberGettable<T> const & number) {
		return (o << number.getNumber());
	}*/

	template <class T>
	class NumberSettable { public: virtual void setNumber(const T& val) noexcept = 0; };

	template <class T>
	class NumberSummable : public NumberGettable<T> {
	public:
		//-----Return value has T type (because I can't return EitherSummable var)----
		T operator+(const NumberSummable& other) const noexcept { return getNumber() + other.getNumber(); }
	};
	template <class T>
	T operator+(const NumberSummable<T>& obj, const T& val) noexcept { return obj.getNumber() + val; }
	template <class T>
	T operator+(const T& val, const NumberSummable<T>& obj) noexcept { return val + obj.getNumber(); }

	template <class T>
	class NumberSubtrative : public NumberGettable<T> {
	public:
		//-----Return value has T type (because I can't return EitherSummable var)----
		T operator-(const NumberSubtrative& other) const noexcept { return getNumber() - other.getNumber(); }
	};
	template <class T>
	T operator-(const NumberSubtrative<T>& obj, const T& val) noexcept { return obj.getNumber() - val; }
	template <class T>
	T operator-(const T& val, const NumberSubtrative<T>& obj) noexcept { return val - obj.getNumber(); }

	template <class T>
	class NumberMultiplicative : public NumberGettable<T> {
	public:
		//-----Return value has T type (because I can't return EitherSummable var)----
		T operator*(const NumberMultiplicative& other) const noexcept { return getNumber() * other.getNumber(); }
	};
	template <class T>
	T operator*(const NumberMultiplicative<T>& obj, const T& val) noexcept { return obj.getNumber() * val; }
	template <class T>
	T operator*(const T& val, const NumberMultiplicative<T>& obj) noexcept { return val * obj.getNumber(); }

	template <class T>
	class NumberDivisible : public NumberGettable<T> {
	public:
		//-----Return value has T type (because I can't return EitherSummable var)----
		T operator/(const NumberDivisible& other) const noexcept { return getNumber() / other.getNumber(); }
	};
	template <class T>
	T operator/(const NumberDivisible<T>& obj, const T& val) noexcept { return obj.getNumber() / val; }
	template <class T>
	T operator/(const T& val, const NumberDivisible<T>& obj) noexcept { return val / obj.getNumber(); }

	template <class T>
	class Algebra : public NumberSummable<T>, 
		public NumberSubtrative<T>,
		public NumberMultiplicative<T>,
		public NumberDivisible<T> {
	public:
	};
}

#endif //ALGEBRA_H

