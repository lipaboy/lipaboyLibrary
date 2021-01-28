#pragma once

//Lipa std
namespace lipaboy_lib {

    /*------------Compare with objects of another type------------*/

    // Too a lot of code production (but more flexible)

    template <class T, class TDerived>
    class AnotherTypeComparingExtender {
    public:
        using DerivedType = TDerived;
        using ValueType = T;
	public:
        // core
        bool operator< (const ValueType& other) const {
            return static_cast<DerivedType const *>(this)->operator< (other);
        }
        bool operator== (const ValueType& other) const {
            return static_cast<DerivedType const *>(this)->operator== (other);
        }

        // extending
        bool operator<= (const ValueType& other) const {
            return ((*this) < other) || ((*this) == other);
        }
        bool operator> (const ValueType& other) const { return !((*this) <= other); }
        bool operator>= (const ValueType& other) const { return !((*this) < other); }
        bool operator!= (const ValueType& other) const { return !((*this) == other); }
	};

    template <class T, class TDerived>
    bool operator< (const T& other, const AnotherTypeComparingExtender<T, TDerived>& obj) { return (obj > other); }
    template <class T, class TDerived>
    bool operator<= (const T& other, const AnotherTypeComparingExtender<T, TDerived>& obj) { return (obj >= other); }
    template <class T, class TDerived>
    bool operator== (const T& other, const AnotherTypeComparingExtender<T, TDerived>& obj) { return (obj == other); }
    template <class T, class TDerived>
    bool operator> (const T& other, const AnotherTypeComparingExtender<T, TDerived>& obj) { return (obj < other); }
    template <class T, class TDerived>
    bool operator>= (const T& other, const AnotherTypeComparingExtender<T, TDerived>& obj) { return (obj <= other); }
    template <class T, class TDerived>
    bool operator!= (const T& other, const AnotherTypeComparingExtender<T, TDerived>& obj) { return (obj != other); }

}

