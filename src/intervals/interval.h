#ifndef INTERVAL_H
#define INTERVAL_H

//TODO: may be can create such class that would get parameters like lambda
//where you can give such labmda: [] (int x, int y) { return x < y; } and would inherit from that class

//TODO: add Args&&

#include "iplenty.h"

#include <functional>

namespace IntervalFuncs {

	//use * to function
	/*std::function<bool(const T& lhs, const T& element)> leftComparison,
	std::function<bool(const T& element, const T& rhs)> rightComparison*/

	template <class T, typename LeftComparison, typename RightComparison>
	class Interval : public IPlenty<T>
	{
	public:
		Interval(const T& left1, const T& right1)
			: leftBorder(left1), rightBorder(right1) {}
		bool in(const T& element) const { return leftComp(leftBorder, element) && rightComp(element, rightBorder); }
		bool out(const T& element) const { return !in(element); }
		bool outLeft(const T& element) const { return !leftComp(leftBorder, element); }
		bool outRight(const T& element) const { return !rightComp(element, rightBorder); }

		virtual bool contains(const T& element) const { return in(element); }

		const T& left() const { return leftBorder; }
		const T& right() const {return rightBorder; }

	protected:
		T leftBorder;
		T rightBorder;
		LeftComparison leftComp;
		RightComparison rightComp;
	};

	//TODO: const double epsilon = 1e-5;	//move into Double

//----------------Cutting off the borders--------------------//

//TODO: move to another package

	template <typename T>
	inline const T& cutOffBorder(const T& value, const T& rightBorder, const T& leftBorder) {
		return ((value < leftBorder) ? leftBorder : ((value > rightBorder) ? rightBorder : value));
	}

	template <typename T>
	inline const T& cutOffRightBorder(const T& value, const T& rightBorder) {
		return ((value > rightBorder) ? rightBorder : value);
	}

	template <typename T>
	inline const T& cutOffLeftBorder(const T& value, const T& leftBorder) {
		return ((value < leftBorder) ? leftBorder : value);
	}

//I don't see any necessary to specialize cutOffBorder for double

//template <>
//inline const double& cutOffBorder(const double& value, const double& rightBorder, const double& leftBorder) {
//	return ((value < leftBorder - epsilon) ? leftBorder : ((value > rightBorder + epsilon) ? rightBorder : value));
//}
//
//template <>
//inline const double& cutOffRightBorder(const double& value, const double& rightBorder) {
//	return ((value > rightBorder) ? rightBorder : value);
//}
//
//template <>
//inline const double& cutOffLeftBorder(const double& value, const double& leftBorder) {
//	return ((value < leftBorder) ? leftBorder : value);
//}

}

#endif // INTERVAL_H
