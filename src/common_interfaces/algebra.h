#ifndef ALGEBRA_H
#define ALGEBRA_H

#include <ostream>

#include <iostream>

namespace lipaboy_lib {

	template <class T>
	class NumberSettable { public: virtual void setNumber(const T& val) noexcept = 0; };

    // I think that the result of operation is defined by operation but not you want

    template <class T, class TDerived>
    class NumberSummable {
	public:
        using DerivedType = TDerived;
        using ValueType = T;
    public:
        ValueType getNumber() const noexcept {
            return static_cast<DerivedType const *>(this)->getNumber();
        }
        DerivedType operator+(const NumberSummable& other) const noexcept {
            return DerivedType(getNumber() + other.getNumber());
        }
        template <class Other>
        ValueType operator+(const NumberSummable<T, Other>& other) const noexcept {
            return getNumber() + other.getNumber();
        }
	};
    template <class T, class TDerived>
    auto operator+(const NumberSummable<T, TDerived>& obj, const T& val) noexcept 
		-> typename NumberSummable<T, TDerived>::DerivedType
	{
        return TDerived(obj.getNumber() + val);
    }
    template <class T, class TDerived>
    auto operator+(const T& val, const NumberSummable<T, TDerived>& obj) noexcept 
		-> typename NumberSummable<T, TDerived>::DerivedType
	{
        return TDerived(val + obj.getNumber());
    }

    template <class T, class TDerived>
    class NumberSubtrative {
	public:
        using ValueType = T;
        using DerivedType = TDerived;
    public:
        ValueType const& getNumber() const noexcept {
            return static_cast<DerivedType const *>(this)->getNumber();
        }
		//-----Return value has T type (because I can't return EitherSummable var)----
        ValueType operator-(const NumberSubtrative& other) const noexcept {
            return getNumber() - other.getNumber();
        }
        template <class Other>
        ValueType operator-(const NumberSubtrative<T, Other>& other) const noexcept {
            return getNumber() - other.getNumber();
        }
	};
    template <class T, class TDerived>
    auto operator-(const NumberSubtrative<T, TDerived>& obj, T const & val) noexcept 
		-> typename NumberSubtrative<T, TDerived>::DerivedType
	{
        return TDerived(obj.getNumber() - val);
    }
    template <class T, class TDerived>
    auto operator-(T const & val, const NumberSubtrative<T, TDerived>& obj) noexcept 
		-> typename NumberSubtrative<T, TDerived>::DerivedType
	{ 
		return TDerived(val - obj.getNumber()); 
	}

    template <class T, class TDerived>
    class NumberMultiplicative {
	public:
        using ValueType = T;
        using DerivedType = TDerived;
    public:
        ValueType const& getNumber() const noexcept {
            return static_cast<DerivedType const *>(this)->getNumber();
        }
		//-----Return value has T type (because I can't return EitherSummable var)----
        ValueType operator*(const NumberMultiplicative& other) const noexcept {
            return getNumber() * other.getNumber();
        }
        template <class Other>
        ValueType operator*(const NumberMultiplicative<T, Other>& other) const noexcept {
            return getNumber() * other.getNumber();
        }
	};
    template <class T, class TDerived>
    auto operator*(const NumberMultiplicative<T, TDerived>& obj, T const & val) noexcept 
		-> typename NumberMultiplicative<T, TDerived>::DerivedType
	{
        return TDerived(obj.getNumber() * val);
    }
    template <class T, class TDerived>
    auto operator*(T const & val, const NumberMultiplicative<T, TDerived>& obj) noexcept 
		-> typename NumberMultiplicative<T, TDerived>::DerivedType
	{ 
		return TDerived(val * obj.getNumber()); 
	}

    template <class T, class TDerived>
    class NumberDivisible {
	public:
        using ValueType = T;
        using DerivedType = TDerived;
    public:
        ValueType const& getNumber() const noexcept {
            return static_cast<DerivedType const *>(this)->getNumber();
        }
		//-----Return value has T type (because I can't return EitherSummable var)----
        ValueType operator/(const NumberDivisible& other) const noexcept {
            return getNumber() / other.getNumber();
        }
        template <class Other>
        ValueType operator/(const NumberDivisible<T, Other>& other) const noexcept {
            return getNumber() / other.getNumber();
        }
	};
    template <class T, class TDerived>
    auto operator/ (const NumberDivisible<T, TDerived>& obj, T const & val) noexcept 
		-> typename NumberDivisible<T, TDerived>::DerivedType
	{
        return TDerived(obj.getNumber() / val);
    }
	template <class T, class TDerived>
	auto operator/ (T const & val, const NumberDivisible<T, TDerived>& obj) noexcept
		-> typename NumberDivisible<T, TDerived>::DerivedType
	{
		return TDerived(obj.getNumber() / val);
	}

    template <class T, class Derived>
    class Algebra :
            public NumberSummable<T, Derived>,
            public NumberSubtrative<T, Derived>,
            public NumberMultiplicative<T, Derived>,
            public NumberDivisible<T, Derived> 
	{
	};

	//---------------Self-operations----------------//

	template <class T, class TDerived>
	class NumberSelfSummable {
	public:
		using ValueType = T;
		using DerivedType = TDerived;
	public:
		ValueType const& getNumber() const noexcept {
			return static_cast<DerivedType const *>(this)->getNumber();
		}
		void setNumber(T const & value) {
			return static_cast<DerivedType *>(this)->setNumber(value);
		}
		//-----Return value has T type (because I can't return EitherSummable var)----
		TDerived const & operator+=(const NumberSelfSummable& other) noexcept {
			setNumber(getNumber() + other.getNumber());
			return static_cast<TDerived const &>(*this);
		}
		template <class Other>
		TDerived const & operator+=(const NumberSelfSummable<T, Other>& other) noexcept {
			setNumber(getNumber() + other.getNumber());
			return static_cast<TDerived const &>(*this);
		}
	};
	template <class T, class TDerived>
	auto operator+=(NumberSelfSummable<T, TDerived> & obj, T const & val) noexcept
		-> typename NumberSelfSummable<T, TDerived>::DerivedType const &
	{
		obj.setNumber(obj.getNumber() + val);
		return static_cast<typename NumberSelfSummable<T, TDerived>::DerivedType const &>(obj);
	}


}

#endif //ALGEBRA_H

