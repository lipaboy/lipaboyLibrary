#ifndef ALGEBRA_H
#define ALGEBRA_H

#include <ostream>

namespace LipaboyLib {

    // TODO: make this class by static polymorphic

    template <class T, class Derived>
	class NumberGettable { 
	public: 
        using DerivedType = Derived;
        using ValueType = T;
    public:
        ValueType const& getNumber() const noexcept {
            return static_cast<Derived const *>(this)->getNumber2();
        }
	};

    template <class T, class Derived>
    inline std::ostream& operator<< (std::ostream& o, NumberGettable<T, Derived> const & number) {
        return (o << number.getNumber());
    }

	template <class T>
	class NumberSettable { public: virtual void setNumber(const T& val) noexcept = 0; };

    template <class T, class TNumberGettable>
    class NumberSummable {
	public:
        using DerivedType = typename TNumberGettable::DerivedType;
        using ValueType = T;
          //  DerivedType;
    public:
        ValueType getNumber__() const noexcept {
            return static_cast<DerivedType const *>(this)->getNumber();
        }

        DerivedType operator+(const NumberSummable& other) const noexcept {
            return DerivedType(getNumber__() + other.getNumber__());
        }
        template <class Other>
        ValueType operator+(const NumberSummable<T, Other>& other) const noexcept {
            return getNumber__() + other.getNumber__();
        }
	};
//    template <class T, class TNumberGettable>
//    typename NumberSummable<T, TNumberGettable>::ValueType
//    operator+(const NumberSummable<T, TNumberGettable>& obj,
//              const typename NumberSummable<T, TNumberGettable>::ValueType& val) noexcept {
//        return obj.getNumber__() + val;
//    }
//    template <class T, class TNumberGettable>
//    typename NumberSummable<T, TNumberGettable>::ValueType
//    operator+(const typename NumberSummable<T, TNumberGettable>::ValueType& val,
//              const NumberSummable<T, TNumberGettable>& obj) noexcept { return val + obj.getNumber__(); }

    template <class T, class TNumberGettable>
    class NumberSubtrative {
	public:
        using ValueType = typename TNumberGettable::ValueType;
        using DerivedType = typename TNumberGettable::DerivedType;
    public:
        ValueType const& getNumber__() const noexcept {
            return static_cast<DerivedType const *>(this)->getNumber();
        }
		//-----Return value has T type (because I can't return EitherSummable var)----
        ValueType operator-(const NumberSubtrative& other) const noexcept {
            return getNumber__() - other.getNumber__();
        }
	};
    template <class T, class TNumberGettable>
    typename NumberSubtrative<T, TNumberGettable>::ValueType
    operator-(const NumberSubtrative<T, TNumberGettable>& obj,
              const typename NumberSubtrative<T, TNumberGettable>::ValueType & val) noexcept {
        return obj.getNumber__() - val;
    }
    template <class T, class TNumberGettable>
    typename NumberSubtrative<T, TNumberGettable>::ValueType
    operator-(const typename NumberSubtrative<T, TNumberGettable>::ValueType & val,
              const NumberSubtrative<T, TNumberGettable>& obj) noexcept { return val - obj.getNumber__(); }

    template <class T, class TNumberGettable>
    class NumberMultiplicative {
	public:
        using ValueType = typename TNumberGettable::ValueType;
        using DerivedType = typename TNumberGettable::DerivedType;
    public:
        ValueType const& getNumber__() const noexcept {
            return static_cast<DerivedType const *>(this)->getNumber();
        }
		//-----Return value has T type (because I can't return EitherSummable var)----
        ValueType operator*(const NumberMultiplicative& other) const noexcept {
            return getNumber__() * other.getNumber__();
        }
	};
    template <class T, class TNumberGettable>
    typename NumberMultiplicative<T, TNumberGettable>::ValueType
    operator*(const NumberMultiplicative<T, TNumberGettable>& obj,
              const typename NumberMultiplicative<T, TNumberGettable>::ValueType & val) noexcept {
        return obj.getNumber__() * val;
    }
    template <class T, class TNumberGettable>
    typename NumberMultiplicative<T, TNumberGettable>::ValueType
    operator*(const typename NumberMultiplicative<T, TNumberGettable>::ValueType & val,
              const NumberMultiplicative<T, TNumberGettable>& obj) noexcept { return val * obj.getNumber__(); }

    template <class T, class TNumberGettable>
    class NumberDivisible {
	public:
        using ValueType = typename TNumberGettable::ValueType;
        using DerivedType = typename TNumberGettable::DerivedType;
    public:
        ValueType const& getNumber__() const noexcept {
            return static_cast<DerivedType const *>(this)->getNumber();
        }
		//-----Return value has T type (because I can't return EitherSummable var)----
        ValueType operator/(const NumberDivisible& other) const noexcept {
            return getNumber__() / other.getNumber__();
        }
	};
    template <class T, class TNumberGettable>
    typename NumberDivisible<T, TNumberGettable>::ValueType
    operator/ (const NumberDivisible<T, TNumberGettable>& obj,
               const typename NumberDivisible<T, TNumberGettable>::ValueType& val) noexcept {
        return obj.getNumber__() / val;
    }
    template <class T, class TNumberGettable>
    typename NumberDivisible<T, TNumberGettable>::ValueType
    operator/(const typename NumberDivisible<T, TNumberGettable>::ValueType& val,
              const NumberDivisible<T, TNumberGettable>& obj) noexcept { return val / obj.getNumber__(); }

    template <class T, class Derived>
    class Algebra :
            public NumberGettable<T, Derived>,
            public NumberSummable<T, NumberGettable<T, Derived>>,
            public NumberSubtrative<T, NumberGettable<T, Derived>>,
            public NumberMultiplicative<T, NumberGettable<T, Derived>>,
            public NumberDivisible<T, NumberGettable<T, Derived>> {
	public:
//        using NumberSummable<T, Derived>::getNumber;
	};
}

#endif //ALGEBRA_H

