#ifndef VECTOR4D_H
#define VECTOR4D_H

#include <QVector>

#include "vector3d.h"
#include "point2d.h"

namespace IsolinesApp {


class Vector4D
{
protected:
    QVector<qreal> vector = QVector<qreal>(4);

public:
    Vector4D() {}
    Vector4D(qreal vect[4]);
    Vector4D(qreal v1, qreal v2, qreal v3, qreal v4);
    Vector4D(Point2D point, qreal v3, qreal v4) : Vector4D(point.x(), point.y(), v3, v4) {}
    Vector4D(Vector3D vec, qreal v4) : Vector4D(vec.x(), vec.y(), vec.z(), v4) {}

    qreal& operator[] (uint32_t index) { return vector[index]; }
    qreal operator[] (uint32_t index) const { return vector[index]; }
    qreal get(uint32_t index) const { return vector[index]; }
    Vector3D toVector3D() const { return Vector3D(vector[0], vector[1], vector[2]); }

    Vector4D operator* (const qreal coef) const;
    //divide on W coordinate (not casting to unitary length)
    const Vector4D& normalize() { (*this) = (*this) * (1.0 / vector[3]); return *this; }
    qreal sumElements() const { return get(0) + get(1) + get(2) + get(3); }

    //Vector4D operator* (const Vector4D& obj) const;
};

//class VectorColumn4D;

class VectorRow4D : public Vector4D
{
public:
    VectorRow4D() : Vector4D() {}
    VectorRow4D(const Vector4D& obj) : Vector4D(obj) {}
    VectorRow4D(qreal v1, qreal v2, qreal v3, qreal v4) : Vector4D(v1, v2, v3, v4) {}
    VectorRow4D(Vector3D vec, qreal v4) : Vector4D(vec, v4) {}
};

class VectorColumn4D : public Vector4D
{
public:
    VectorColumn4D() : Vector4D() {}
    VectorColumn4D(qreal vect[4]) : Vector4D(vect) {}
    VectorColumn4D(const Vector4D& obj) : Vector4D(obj) {}
    VectorColumn4D(qreal v1, qreal v2, qreal v3, qreal v4) : Vector4D(v1, v2, v3, v4) {}
    VectorColumn4D(Vector3D vec, qreal v4) : Vector4D(vec, v4) {}
    VectorColumn4D(Point2D point, qreal v3, qreal v4) : Vector4D(point, v3, v4) {}
};

inline VectorColumn4D transposeVec(const VectorRow4D& obj) { return VectorColumn4D(obj); }
inline VectorRow4D transposeVec(const VectorColumn4D& obj) { return VectorRow4D(obj); }

qreal operator* (const VectorRow4D& v1, const VectorColumn4D& v2);



}

#endif // VECTOR4D_H
