//
// Created by david on 5/4/26.
//

#include <QOpenGLDebugLogger>
#include "CubicBezier2D.h"

CubicBezier2D::CubicBezier2D() :
CubicBezier2D(
    QVector2D(0.0f, 0.5f),
    QVector2D(1.0f/3, 0.5f),
    QVector2D(2.0f/3, 0.5f),
    QVector2D(1.0f, 0.5f))
{}

CubicBezier2D::CubicBezier2D(QVector2D p0, QVector2D p1, QVector2D p2, QVector2D p3)
{
    this->p0 = p0;
    this->p1 = p1;
    this->p2 = p2;
    this->p3 = p3;
}

QVector2D CubicBezier2D::at(float t)
{
     return (1-t)*(1-t)*(1-t)*p0 + 3*(1-t)*(1-t)*t*p1 + 3*(1-t)*t*t*p2 + t*t*t*p3;
}

QVector2D CubicBezier2D::derivativeAt(float t)
{
    return 3*(1-t)*(1-t)*(p1-p0) + 6*(1-t)*t*(p2-p1) + 3*t*t*(p3-p2);
}

QVector2D CubicBezier2D::secondDerivativeAt(float t)
{
    return 6*(1-t)*(p2-2*p1+p0) + 6*t*(p3-2*p2+p1);
}

QVector2D CubicBezier2D::unitNormalAt(float t)
{
    QVector2D v = derivativeAt(t);
    v.normalize();
    QVector2D n(-v.y(), v.x());
    return n;
}

QVector2D CubicBezier2D::dNormalAt(float t)
{
    QVector2D v = derivativeAt(t);
    QVector2D a = secondDerivativeAt(t);

    float len2 = v.lengthSquared();
    float len = sqrt(len2);

    QVector2D aperp(-a.y(), a.x());

    QVector2D dNdt = aperp/len - unitNormalAt(t) * (QVector2D::dotProduct(v, a) / len2);

    return dNdt;
}

void CubicBezier2D::setPoints(QVector2D p0, QVector2D p1, QVector2D p2, QVector2D p3)
{
    this->p0 = p0;
    this->p1 = p1;
    this->p2 = p2;
    this->p3 = p3;
}

QVector2D CubicBezier2D::getPoint(int i) {
    switch(i) {
    case 0: return p0; case 1: return p1;
    case 2: return p2; case 3: return p3;
    default: return p0;
    }
}
void CubicBezier2D::setPoint(int i, QVector2D p) {
    switch(i) {
    case 0: setP0(p); break; case 1: setP1(p); break;
    case 2: setP2(p); break; case 3: setP3(p); break;
    default: break;
    }
}


