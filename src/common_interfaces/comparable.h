#ifndef COMPARABLE_H
#define COMPARABLE_H

//Lipa std
namespace lipaboy_lib {

    // INFO: ComparatorExtender extends the operators for class.
	// ISSUE to class: class must have two operators: operator== and operator<

    template <class Derived>
    class ComparatorExtender {
	public:
        bool operator< (const ComparatorExtender& other) const {
            return static_cast<Derived const *>(this)->operator< (*static_cast<Derived const *>(&other));
        }
        bool operator<= (const ComparatorExtender& other) const {
			return ((*this) < other) || ((*this) == other);
		}
        bool operator== (const ComparatorExtender& other) const {
            return static_cast<Derived const *>(this)->operator== (*static_cast<Derived const *>(&other));
        }

        bool operator> (const ComparatorExtender& other) const { return !((*this) <= other); }
        bool operator>= (const ComparatorExtender& other) const { return !((*this) < other); }
        bool operator!= (const ComparatorExtender& other) const { return !((*this) == other); }
	};

}

#endif //COMPARABLE_H
#pragma once
