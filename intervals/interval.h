#ifndef INTERVAL_H
#define INTERVAL_H

//TODO: may be can create such class that would get parameters like lambda
//where you can give such labmda: [] (int x, int y) { return x < y; } and would inherit from that class

//TODO: shake maths package with this one

#include "segment.h"

namespace IntervalFuncs {

template <typename T>
T cutOffBorder(T value, T rightBorder, T leftBorder = static_cast<T>(0)) {
    return ((value < leftBorder) ? leftBorder : ((value > rightBorder) ? rightBorder : value));
}

template <typename T>
T cutOffLeftBorder(T value, T leftBorder = static_cast<T>(0)) {
    return ((value < leftBorder) ? leftBorder : value);
}

//TODO: QPoint
//template <>
//QPoint cutOffBorder(T value, T rightBorder, T leftBorder = static_cast<T>(0)) {
//    return ((value < leftBorder) ? leftBorder : ((value > rightBorder) ? rightBorder : value));
//}

template <class T>
class Interval
{
public:
    Interval(T left1, T right1) : leftBorder(left1), rightBorder(right1) {}
    bool in(T element) const { return (element > leftBorder) && (element < rightBorder); }
    bool out(T element) const { return !in(element); }
    bool outLeft(T element) const { return (element <= leftBorder); }
    bool outRight(T element) const { return (element >= rightBorder); }

    T left() const { return leftBorder; }
    T right() const {return rightBorder; }

protected:
    T leftBorder;
    T rightBorder;
};

}

#endif // INTERVAL_H
