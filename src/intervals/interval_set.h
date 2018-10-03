#pragma once

#include "interval.h"

#include <unordered_set>

namespace lipaboy_lib {

	using std::unordered_set;

	//template <class T,
	//	class LeftComparator,
	//	class RightComparator
	//>
	//auto unionIntervals(
	//	Interval<T, LeftComparator, RightComparator> first, 
	//	Interval<T, LeftComparator, RightComparator> second
	//)
	//	-> Interval<T, LeftComparator, RightComparator>
	//{
	//	using IntevalType = Interval<T, LeftComparator, RightComparator>;
	//	//if 
	//	return first;
	//}

	template <class T,
		class LeftComparator,
		class RightComparator,
		class TLeftBorder = T,
		class TRightBorder = T
	>
		class IntervalSet {
		public:
			using ValueType = IntervalBase<T, LeftComparator, RightComparator, TLeftBorder, TRightBorder>;
			using ContainerType = unordered_set<ValueType>;

		public:


		private:
			ContainerType set_;
	};

}
