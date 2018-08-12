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
    typename NumberSummable<T, TDerived>::DerivedType
    operator+(const NumberSummable<T, TDerived>& obj, const T& val) noexcept {
        return TDerived(obj.getNumber() + val);
    }
    template <class T, class TDerived>
    typename NumberSummable<T, TDerived>::DerivedType
    operator+(const T& val, const NumberSummable<T, TDerived>& obj) noexcept {
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
    typename NumberSubtrative<T, TDerived>::ValueType
    operator-(const NumberSubtrative<T, TDerived>& obj,
              const typename NumberSubtrative<T, TDerived>::ValueType & val) noexcept {
        return obj.getNumber() - val;
    }
    template <class T, class TDerived>
    typename NumberSubtrative<T, TDerived>::ValueType
    operator-(const typename NumberSubtrative<T, TDerived>::ValueType & val,
              const NumberSubtrative<T, TDerived>& obj) noexcept { return val - obj.getNumber(); }

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
    typename NumberMultiplicative<T, TDerived>::ValueType
    operator*(const NumberMultiplicative<T, TDerived>& obj,
              const typename NumberMultiplicative<T, TDerived>::ValueType & val) noexcept {
        return obj.getNumber() * val;
    }
    template <class T, class TDerived>
    typename NumberMultiplicative<T, TDerived>::ValueType
    operator*(const typename NumberMultiplicative<T, TDerived>::ValueType & val,
              const NumberMultiplicative<T, TDerived>& obj) noexcept { return val * obj.getNumber(); }

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
    typename NumberDivisible<T, TDerived>::ValueType
    operator/ (const NumberDivisible<T, TDerived>& obj,
               const typename NumberDivisible<T, TDerived>::ValueType& val) noexcept {
        return obj.getNumber() / val;
    }
    template <class T, class TDerived>
    typename NumberDivisible<T, TDerived>::ValueType
    operator/(const typename NumberDivisible<T, TDerived>::ValueType& val,
              const NumberDivisible<T, TDerived>& obj) noexcept { return val / obj.getNumber(); }

    template <class T, class Derived>
    class Algebra :
            public NumberSummable<T, Derived>,
            public NumberSubtrative<T, Derived>,
            public NumberMultiplicative<T, Derived>,
            public NumberDivisible<T, Derived> {
    public:
	};
}

#endif //ALGEBRA_H

