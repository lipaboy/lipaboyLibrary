#ifndef CUT_OFF_BORDERS_H
#define CUT_OFF_BORDERS_H

#include <segment.h>

namespace LipaboyLib {

	//TODO: move to another package (but what's package??)

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

	template <typename T>
	T cutOffBorder(T value, const Segment<T>& border) {
		return ((border.outLeft(value)) ? border.left() : ((border.outRight(value)) ? border.right() : value));
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

#endif //CUT_OFF_BORDERS_H
