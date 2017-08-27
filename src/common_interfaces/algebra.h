#ifndef ALGEBRA_H
#define ALGEBRA_H

namespace LipaboyLib {

	template <class T>
	class EitherNumberGettable { public: virtual T const& getNumber() const noexcept = 0; };
	template <class T>
	class EitherNumberSettable { public: virtual void setNumber(const T& val) noexcept = 0; };

	template <class T>
	class EitherSummable : public EitherNumberGettable<T> {
	public:
		//-----Return value has T type (because I can't return EitherSummable var)----
		T operator+(const EitherSummable& other) const noexcept { return getNumber() + other.getNumber(); }
	};
	template <class T>
	T operator+(const EitherSummable<T>& obj, const T& val) noexcept { return obj.getNumber() + val; }
	template <class T>
	T operator+(const T& val, const EitherSummable<T>& obj) noexcept { return val + obj.getNumber(); }

	template <class T>
	class Algebra : public EitherSummable<T> {
	public:
	};
}

#endif //ALGEBRA_H

