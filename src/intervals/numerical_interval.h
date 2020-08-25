#pragma once

#include "interval.h"

namespace lipaboy_lib {

	template <
		class T, 
		class LeftComparator, 
		class RightComparator,
		class DifferenceFunc = std::minus<T>
	>
	class NumericalInterval : public Interval<T, LeftComparator, RightComparator>
	{
    private:
        using SuperType = Interval<T, LeftComparator, RightComparator>;
	public:
		NumericalInterval(const T& leftBorder, const T& rightBorder)
            : SuperType(leftBorder, rightBorder) {}

        T length() { return DifferenceFunc()(SuperType::right(), SuperType::left()); }

	};

	template <class T>
	using OpenNumericalInterval = NumericalInterval<T, std::less<T>, std::less<T> >;

	template <class T>
	using CloseNumericalInterval = NumericalInterval<T, std::less_equal<T>, std::less_equal<T> >;

}
