#ifndef ALGEBRA_H
#define ALGEBRA_H

namespace LipaboyLib {

	template <class T>
	class Elemable {
	public:
		Elemable(const T& val) noexcept : elem(val) {}
		Elemable const & operator=(T const & val) noexcept { set(val); return *this; }
		void set(const T& val) noexcept { elem = val; }		//I can't mark method as noexcept because I don't sure that operator= won't throw exception
		const T& get() const noexcept { return elem; }
	private:
		T elem;
	};

	template <class T>
	class EitherSummable : public Elemable<T> {
	public:
		EitherSummable(const T& val) noexcept : Elemable(val) { }
		T operator+(const EitherSummable& other) const noexcept { return get() + other.get(); }
	};
	template <class T>
	T operator+(const EitherSummable<T>& obj, const T& val) noexcept { return obj.get() + val; }
	template <class T>
	T operator+(const T& val, const EitherSummable<T>& obj) noexcept { return val + obj.get(); }

	template <class T>
	class Algebra : public EitherSummable<T> {
	public:
		Algebra(const T& val) noexcept : EitherSummable(val) {}
	};
}

#endif //ALGEBRA_H

