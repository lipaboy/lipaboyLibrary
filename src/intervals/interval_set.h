#pragma once

//#include "interval.h"
#include "numerical_interval.h"

#include <unordered_set>

namespace lipaboy_lib {

	using std::unordered_set;

	template <class T,
		class LeftComparator,
		class RightComparator
	>
	auto unionIntervals(
		NumericalInterval<T, LeftComparator, RightComparator> const & first, 
		NumericalInterval<T, LeftComparator, RightComparator> const & second
	)
		-> Interval<T, LeftComparator, RightComparator>
	{
		using IntevalType = Interval<T, LeftComparator, RightComparator>;
		
		if (first.length() < T(0)) 

		return first;
	}

	template <class T,
		class LeftComparator,
		class RightComparator,
		class DifferenceFunc = std::minus<T>
	>
		class NumericalIntervalSet {
		public:
			using ValueType = NumericalInterval<T, LeftComparator, RightComparator, DifferenceFunc>;
			using ContainerType = unordered_set<ValueType>;

		public:


		private:
			ContainerType set_;
	};

}
