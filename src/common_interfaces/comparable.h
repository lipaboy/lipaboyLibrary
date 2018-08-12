#ifndef COMPARABLE_H
#define COMPARABLE_H

//Lipa std
namespace lipaboy_lib {

    // Hint: EitherComparable with other object types

    template <class Derived>
    class ComparatorExtender {
	public:
        bool operator< (const ComparatorExtender& other) const {
            return static_cast<Derived const *>(this)->operator< (*dynamic_cast<Derived const *>(&other));
        }
        bool operator<= (const ComparatorExtender& other) const {
			return ((*this) < other) || ((*this) == other);
		}
        bool operator== (const ComparatorExtender& other) const {
            return static_cast<Derived const *>(this)->operator== (*dynamic_cast<Derived const *>(&other));
        }

//        bool operator> (const ComparatorExtender& other) const { return !((*this) <= other); }
//        bool operator>= (const ComparatorExtender& other) const { return !((*this) < other); }
        bool operator!= (const ComparatorExtender& other) const { return !((*this) == other); }
	};

}

#endif //COMPARABLE_H
#pragma once
