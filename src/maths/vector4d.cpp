#include "vector4d.h"

namespace LipaboyMaths {

Vector4D::Vector4D(double vect1[4])
{
    vect[0] = vect1[0];
    vect[1] = vect1[1];
    vect[2] = vect1[2];
    vect[3] = vect1[3];
}

Vector4D::Vector4D(double v1, double v2, double v3, double v4)
{
    vect[0] = v1;
    vect[1] = v2;
    vect[2] = v3;
    vect[3] = v4;
}

Vector4D Vector4D::operator*(const double coef) const
{
    Vector4D newVec({0.0, 0.0, 0.0, 0.0});

    for (int i = 0; i < 4; i++) {
        newVec.vect[i] = this -> vect[i] * coef;
    }

    return newVec;
}

//Vector4D Vector4D::operator*(const Vector4D &obj) const
//{
//    Vector4D newVec({0.0, 0.0, 0.0, 0.0});

//    for (int i = 0; i < 4; i++)
//        newVec[i] = this -> vect[i] * obj.vect[i];

//    return newVec;
//}

double operator*(const VectorRow4D &v1, const VectorColumn4D &v2)
{
    double res = 0.0;
    for (int i = 0; i < 4; i++) {
        res += v1[i] * v2[i];
    }

    return res;
}


}
