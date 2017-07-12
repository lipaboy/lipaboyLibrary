#ifndef VECTOR2D_H
#define VECTOR2D_H

#include <QPoint>
#include <cmath>

namespace IsolinesApp {


class Vector2D
{
public:
    Vector2D(QPoint point) : x(point.x()), y(point.y()) {}

    qreal length() const { return std::sqrt(x * x + y * y); }
    qreal squaredLength() const { return (x * x + y * y); }

private:
    qreal x;
    qreal y;
};

}

#endif // VECTOR2D_H
