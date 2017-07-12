#include "vector4d.h"

namespace IsolinesApp {

Vector4D::Vector4D(qreal vect[4])
{
    vector[0] = vect[0];
    vector[1] = vect[1];
    vector[2] = vect[2];
    vector[3] = vect[3];
}

Vector4D::Vector4D(qreal v1, qreal v2, qreal v3, qreal v4)
{
    vector[0] = v1;
    vector[1] = v2;
    vector[2] = v3;
    vector[3] = v4;
}

Vector4D Vector4D::operator*(const qreal coef) const
{
    Vector4D newVec({0.0, 0.0, 0.0, 0.0});

    for (int i = 0; i < 4; i++) {
        newVec.vector[i] = this -> vector[i] * coef;
    }

    return newVec;
}

//Vector4D Vector4D::operator*(const Vector4D &obj) const
//{
//    Vector4D newVec({0.0, 0.0, 0.0, 0.0});

//    for (int i = 0; i < 4; i++)
//        newVec[i] = this -> vector[i] * obj.vector[i];

//    return newVec;
//}

qreal operator*(const VectorRow4D &v1, const VectorColumn4D &v2)
{
    qreal res = 0.0;
    for (int i = 0; i < 4; i++) {
        res += v1[i] * v2[i];
    }

    return res;
}


}
