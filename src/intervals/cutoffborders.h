#ifndef CUT_OFF_BORDERS_H
#define CUT_OFF_BORDERS_H

namespace lipaboy_lib {

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

}

#endif //CUT_OFF_BORDERS_H
