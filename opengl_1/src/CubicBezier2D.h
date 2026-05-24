//
// Created by david on 5/4/26.
//

#ifndef OPENGL_1_BEZIER2D_H
#define OPENGL_1_BEZIER2D_H
#include <QVector2D>


class CubicBezier2D
{
    QVector2D p0, p1, p2, p3;

    public:
    CubicBezier2D();
    CubicBezier2D(QVector2D p0, QVector2D p1, QVector2D p2, QVector2D p3);
    QVector2D at(float t);
    QVector2D derivativeAt(float t);
    QVector2D secondDerivativeAt(float t);
    QVector2D unitNormalAt(float t);
    QVector2D dNormalAt(float t);

    void setPoints(QVector2D p0, QVector2D p1, QVector2D p2, QVector2D p3);

    inline void setP0(QVector2D p) {p0.setX(p.x()); p0.setY(p.y());};
    inline void setP1(QVector2D p) {p1.setX(p.x()); p1.setY(p.y());};
    inline void setP2(QVector2D p) {p2.setX(p.x()); p2.setY(p.y());};
    inline void setP3(QVector2D p) {p3.setX(p.x()); p3.setY(p.y());};
    inline QVector2D getP0() {return p0;};
    inline QVector2D getP1() {return p1;};
    inline QVector2D getP2() {return p2;};
    inline QVector2D getP3() {return p3;};
};


#endif //OPENGL_1_BEZIER2D_H