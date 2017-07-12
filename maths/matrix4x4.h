#ifndef MATRIX4x4_H
#define MATRIX4x4_H

#include "vector4d.h"

#include <QVector>

namespace IsolinesApp {

//TODO: rename namespace to Maths namespace

class Matrix4x4
{
private:
    QVector<qreal> matrix = QVector<qreal>(4 * 4);

public:
    //Matrix4x4(const QVector<qreal>& matr);
    Matrix4x4() {}
    Matrix4x4(float matr[16]);
    Matrix4x4(qreal matr[16]);
    Matrix4x4(const Matrix4x4& obj);
    Matrix4x4(const VectorColumn4D& v1, const VectorColumn4D& v2, const VectorColumn4D& v3,
              const VectorColumn4D& v4);
    Matrix4x4(const VectorRow4D& v1, const VectorRow4D& v2, const VectorRow4D& v3,
              const VectorRow4D& v4);

    VectorColumn4D operator* (const VectorColumn4D& vect) const;
    Matrix4x4 operator* (const Matrix4x4& vect) const;
    const Matrix4x4& operator*= (const Matrix4x4& vect) { return ((*this) = (*this) * vect); }
    Matrix4x4 operator- (const Matrix4x4& vect) const;
    Matrix4x4 operator+ (const Matrix4x4& vect) const;
    const Matrix4x4& operator*= (const qreal coef);
    Matrix4x4 operator* (const qreal coef) { return (Matrix4x4(*this) *= coef); }

    qreal get(uint32_t row, uint32_t column) const { return matrix[row * 4 + column]; }
    qreal& get(uint32_t row, uint32_t column) { return matrix[row * 4 + column]; }
    void set(uint32_t row, uint32_t column, qreal value) { matrix[row * 4 + column] = value; }
    void add(uint32_t row, uint32_t column, qreal value) { matrix[row * 4 + column] += value; }
    void transpose();

    static Matrix4x4 getEye();
    static Matrix4x4 getEye3D();
    static Matrix4x4 getDualMatrix(Vector3D vect);
    static Matrix4x4 getDiagonal(Vector4D vect);
};

Matrix4x4 operator* (const VectorColumn4D& v1, const VectorRow4D& v2);

}

#endif // MATRIX4D_H
