#ifndef EITHER_COMPARABLE_H
#define EITHER_COMPARABLE_H

//Lipa std
namespace lipaboy_lib {


	/*------------Too a lot of code production (but more flexible)------------*/

	// TODO: swap (*this) and other by places (more beautifully)

	// EitherComparable with other object types
    template <class T, class TDerived>
	class EitherComparable {
    public:
        using DerivedType = TDerived;
        using ValueType = T;
	public:
        bool operator< (const ValueType& other) const {
            return static_cast<DerivedType const *>(this)->operator< (other);
        }
        bool operator<= (const ValueType& other) const {
			return ((*this) < other) || ((*this) == other);
		}
        bool operator== (const ValueType& other) const {
            return static_cast<DerivedType const *>(this)->operator== (other);
        }

		bool operator> (const ValueType& other) const { return !((*this) <= other); }
		bool operator>= (const ValueType& other) const { return !((*this) < other); }
        bool operator!= (const ValueType& other) const { return !((*this) == other); }
	};

    template <class T, class TDerived>
    bool operator< (const T& other, const EitherComparable<T, TDerived>& obj) { return (obj > other); }
    template <class T, class TDerived>
    bool operator<= (const T& other, const EitherComparable<T, TDerived>& obj) { return (obj >= other); }
    template <class T, class TDerived>
    bool operator== (const T& other, const EitherComparable<T, TDerived>& obj) { return (obj == other); }
    template <class T, class TDerived>
    bool operator> (const T& other, const EitherComparable<T, TDerived>& obj) { return (obj < other); }
    template <class T, class TDerived>
    bool operator>= (const T& other, const EitherComparable<T, TDerived>& obj) { return (obj <= other); }
    template <class T, class TDerived>
    bool operator!= (const T& other, const EitherComparable<T, TDerived>& obj) { return (obj != other); }



}

#endif //EITHER_COMPARABLE_H
