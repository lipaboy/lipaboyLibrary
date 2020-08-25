#ifndef ALGEBRA_H
#define ALGEBRA_H

#include <ostream>

#include <iostream>

namespace lipaboy_lib {

	template <class T>
	class NumberSettable { public: virtual void setNumber(const T& val)  = 0; };

    // I think that the result of operation is defined by operation but not you want

	//                 |
	//               --+--
	//                 |

    template <class T, class TDerived>
    class NumberSummable {
	public:
        using DerivedType = TDerived;
        using ValueType = T;
    public:
        ValueType getNumber() const  {
            return static_cast<DerivedType const *>(this)->getNumber();
        }
        DerivedType operator+(const NumberSummable& other) const  {
            return DerivedType(getNumber() + other.getNumber());
        }
        template <class Other>
        ValueType operator+(const NumberSummable<T, Other>& other) const  {
            return getNumber() + other.getNumber();
        }
	};
    template <class T, class TDerived>
    auto operator+(const NumberSummable<T, TDerived>& obj, const T& val)  
		-> typename NumberSummable<T, TDerived>::DerivedType
	{
        return TDerived(obj.getNumber() + val);
    }
    template <class T, class TDerived>
    auto operator+(const T& val, const NumberSummable<T, TDerived>& obj)  
		-> typename NumberSummable<T, TDerived>::DerivedType
	{
        return TDerived(val + obj.getNumber());
    }

	//                 
	//               _____
	//                 

    template <class T, class TDerived>
    class NumberSubtractive {
	public:
        using ValueType = T;
        using DerivedType = TDerived;
    public:
        ValueType const& getNumber() const  {
            return static_cast<DerivedType const *>(this)->getNumber();
        }
		//-----Return value has T type (because I can't return EitherSummable var)----
        ValueType operator-(const NumberSubtractive& other) const  {
            return getNumber() - other.getNumber();
        }
        template <class Other>
        ValueType operator-(const NumberSubtractive<T, Other>& other) const  {
            return getNumber() - other.getNumber();
        }
	};
    template <class T, class TDerived>
    auto operator-(const NumberSubtractive<T, TDerived>& obj, T const & val)  
		-> typename NumberSubtractive<T, TDerived>::DerivedType
	{
        return TDerived(obj.getNumber() - val);
    }
    template <class T, class TDerived>
    auto operator-(T const & val, const NumberSubtractive<T, TDerived>& obj)  
		-> typename NumberSubtractive<T, TDerived>::DerivedType
	{ 
		return TDerived(val - obj.getNumber()); 
	}

	//               \ V /
	//               >-+-<
	//               / | \

    template <class T, class TDerived>
    class NumberMultiplicative {
	public:
        using ValueType = T;
        using DerivedType = TDerived;
    public:
        ValueType const& getNumber() const  {
            return static_cast<DerivedType const *>(this)->getNumber();
        }
		//-----Return value has T type (because I can't return EitherSummable var)----
        ValueType operator*(const NumberMultiplicative& other) const  {
            return getNumber() * other.getNumber();
        }
        template <class Other>
        ValueType operator*(const NumberMultiplicative<T, Other>& other) const  {
            return getNumber() * other.getNumber();
        }
	};
    template <class T, class TDerived>
    auto operator*(const NumberMultiplicative<T, TDerived>& obj, T const & val)  
		-> typename NumberMultiplicative<T, TDerived>::DerivedType
	{
        return TDerived(obj.getNumber() * val);
    }
    template <class T, class TDerived>
    auto operator*(T const & val, const NumberMultiplicative<T, TDerived>& obj)  
		-> typename NumberMultiplicative<T, TDerived>::DerivedType
	{ 
		return TDerived(val * obj.getNumber()); 
	}

	//                 .
	//               _____
	//                 .

    template <class T, class TDerived>
    class NumberDivisible {
	public:
        using ValueType = T;
        using DerivedType = TDerived;
    public:
        ValueType const& getNumber() const  {
            return static_cast<DerivedType const *>(this)->getNumber();
        }
		//-----Return value has T type (because I can't return EitherSummable var)----
        ValueType operator/(const NumberDivisible& other) const  {
            return getNumber() / other.getNumber();
        }
        template <class Other>
        ValueType operator/(const NumberDivisible<T, Other>& other) const  {
            return getNumber() / other.getNumber();
        }
	};
    template <class T, class TDerived>
    auto operator/ (const NumberDivisible<T, TDerived>& obj, T const & val)  
		-> typename NumberDivisible<T, TDerived>::DerivedType
	{
        return TDerived(obj.getNumber() / val);
    }
	template <class T, class TDerived>
	auto operator/ (T const & val, const NumberDivisible<T, TDerived>& obj) 
		-> typename NumberDivisible<T, TDerived>::DerivedType
	{
		return TDerived(obj.getNumber() / val);
	}

	//-----------------------------------------------------
	//---------------------Algebra-------------------------
	//-----------------------------------------------------

    template <class T, class Derived>
    class OperationAlgebra :
            public NumberSummable<T, Derived>,
            public NumberSubtractive<T, Derived>,
            public NumberMultiplicative<T, Derived>,
            public NumberDivisible<T, Derived> 
	{
	};

	//---------------Self-operations----------------//

	//                 |   ____
	//               --+-- ____
	//                 |

	template <class T, class TDerived>
	class NumberSelfSummable {
	public:
		using ValueType = T;
		using DerivedType = TDerived;
	public:
		ValueType const& getNumber() const  {
			return static_cast<DerivedType const *>(this)->getNumber();
		}
		void setNumber(T const & value) {
			return static_cast<DerivedType *>(this)->setNumber(value);
		}
		TDerived const & operator+=(const NumberSelfSummable& other)  {
			setNumber(getNumber() + other.getNumber());
			return static_cast<TDerived const &>(*this);
		}
		template <class Other>
		TDerived const & operator+=(const NumberSelfSummable<T, Other>& other)  {
			setNumber(getNumber() + other.getNumber());
			return static_cast<TDerived const &>(*this);
		}
	};
	template <class T, class TDerived>
	auto operator+=(NumberSelfSummable<T, TDerived> & obj, T const & val) 
		-> typename NumberSelfSummable<T, TDerived>::DerivedType const &
	{
		obj.setNumber(obj.getNumber() + val);
		return static_cast<typename NumberSelfSummable<T, TDerived>::DerivedType const &>(obj);
	}


	//                      ______
	//               -----  ______
	//                      

	template <class T, class TDerived>
	class NumberSelfSubtractive {
	public:
		using ValueType = T;
		using DerivedType = TDerived;
	public:
		ValueType const& getNumber() const  {
			return static_cast<DerivedType const *>(this)->getNumber();
		}
		void setNumber(T const & value) {
			return static_cast<DerivedType *>(this)->setNumber(value);
		}
		TDerived const & operator-=(const NumberSelfSubtractive& other)  {
			setNumber(getNumber() - other.getNumber());
			return static_cast<TDerived const &>(*this);
		}
		template <class Other>
		TDerived const & operator-=(const NumberSelfSubtractive<T, Other>& other)  {
			setNumber(getNumber() - other.getNumber());
			return static_cast<TDerived const &>(*this);
		}
	};
	template <class T, class TDerived>
	auto operator-=(NumberSelfSubtractive<T, TDerived> & obj, T const & val) 
		-> typename NumberSelfSubtractive<T, TDerived>::DerivedType const &
	{
		obj.setNumber(obj.getNumber() - val);
		return static_cast<typename NumberSelfSubtractive<T, TDerived>::DerivedType const &>(obj);
	}


	//               \ V /  ______
	//               >-+-<  ______
	//               / | \

	template <class T, class TDerived>
	class NumberSelfMultiplicative {
	public:
		using ValueType = T;
		using DerivedType = TDerived;
	public:
		ValueType const& getNumber() const  {
			return static_cast<DerivedType const *>(this)->getNumber();
		}
		void setNumber(T const & value) {
			return static_cast<DerivedType *>(this)->setNumber(value);
		}
		TDerived const & operator*=(const NumberSelfMultiplicative& other)  {
			setNumber(getNumber() * other.getNumber());
			return static_cast<TDerived const &>(*this);
		}
		template <class Other>
		TDerived const & operator*=(const NumberSelfMultiplicative<T, Other>& other)  {
			setNumber(getNumber() * other.getNumber());
			return static_cast<TDerived const &>(*this);
		}
	};
	template <class T, class TDerived>
	auto operator*=(NumberSelfMultiplicative<T, TDerived> & obj, T const & val) 
		-> typename NumberSelfMultiplicative<T, TDerived>::DerivedType const &
	{
		obj.setNumber(obj.getNumber() * val);
		return static_cast<typename NumberSelfMultiplicative<T, TDerived>::DerivedType const &>(obj);
	}

	//                 .    ______
	//               _____  ______
	//                 .

	template <class T, class TDerived>
	class NumberSelfDivisible {
	public:
		using ValueType = T;
		using DerivedType = TDerived;
	public:
		ValueType const& getNumber() const  {
			return static_cast<DerivedType const *>(this)->getNumber();
		}
		void setNumber(T const & value) {
			return static_cast<DerivedType *>(this)->setNumber(value);
		}
		TDerived const & operator/=(const NumberSelfDivisible& other)  {
			setNumber(getNumber() / other.getNumber());
			return static_cast<TDerived const &>(*this);
		}
		template <class Other>
		TDerived const & operator/=(const NumberSelfDivisible<T, Other>& other)  {
			setNumber(getNumber() / other.getNumber());
			return static_cast<TDerived const &>(*this);
		}
	};
	template <class T, class TDerived>
	auto operator/=(NumberSelfDivisible<T, TDerived> & obj, T const & val) 
		-> typename NumberSelfDivisible<T, TDerived>::DerivedType const &
	{
		obj.setNumber(obj.getNumber() / val);
		return static_cast<typename NumberSelfDivisible<T, TDerived>::DerivedType const &>(obj);
	}


	//-----------------------------------------------------
	//---------------------Self-Algebra-------------------------
	//-----------------------------------------------------

	template <class T, class Derived>
	class SelfOperationAlgebra :
		public NumberSelfSummable<T, Derived>,
		public NumberSelfSubtractive<T, Derived>,
		public NumberSelfMultiplicative<T, Derived>,
		public NumberSelfDivisible<T, Derived>
	{
	};


}

#endif //ALGEBRA_H

