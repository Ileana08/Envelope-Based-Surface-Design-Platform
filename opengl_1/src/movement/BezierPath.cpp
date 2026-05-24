#include "BezierPath.h"

BezierPath::BezierPath()
    : controlPoints(QVector<ControlPoint*>())
{}

BezierPath::BezierPath(QVector<ControlPoint*> controlPoints)
    : controlPoints(controlPoints)
{}

QVector3D BezierPath::getPathAt(float t)
{
// cubic: C(t) = (1-t)^3 P0 + 3 (1-t)^2 t P1 + 3 (1-t) t^2 P2 + t^3 P3

    QVector3D pt = QVector3D();
    QVector3D p0 = controlPoints[0]->getPosition();
    QVector3D p1 = controlPoints[1]->getPosition();
    QVector3D p2 = controlPoints[2]->getPosition();
    QVector3D p3 = controlPoints[3]->getPosition();
    pt = (1-t) * (1-t) * (1-t) * p0 + 3 * (1-t) * (1-t) * t * p1 + 3 * (1-t) * t * t * p2 + t * t * t * p3;
    return pt;
}

void BezierPath::updateVertexArr()
{
    vertexArr.clear();
    QVector3D color(0,0,1);

    for(int i=0; i<=sectors; ++i){
        float t = (float) i / sectors;
        vertexArr.append(Vertex(getPathAt(t), color, color));
    }
}

QVector3D BezierPath::getDerivativeAt(float t)
{
    QVector3D tangent = QVector3D();
    QVector3D p0 = controlPoints[0]->getPosition();
    QVector3D p1 = controlPoints[1]->getPosition();
    QVector3D p2 = controlPoints[2]->getPosition();
    QVector3D p3 = controlPoints[3]->getPosition();
    float first_multiplier = -1 * 3 * (1-t) * (1-t);
    float second_multipier = 3 * (1-t) * (1-t) - 6 * (1-t) * t;
    float third_multipier = 6 * (1-t) * t - 3 * t * t;
    float fourth_multiplier = 3 * t * t;
    tangent = first_multiplier * p0 + second_multipier * p1 + third_multipier * p2 + fourth_multiplier * p3;
    return tangent;
}

QVector3D BezierPath::getDerivative2At(float t) {
    QVector3D acc = QVector3D();
    QVector3D p0 = controlPoints[0]->getPosition();
    QVector3D p1 = controlPoints[1]->getPosition();
    QVector3D p2 = controlPoints[2]->getPosition();
    QVector3D p3 = controlPoints[3]->getPosition();
    float first_multiplier = 6 * (1-t);
    float second_multipier = 6 * t - 12 * (1-t);
    float third_multipier = 6 * (1-t) - 12 * t;
    float fourth_multiplier = 6 * t;
    acc = first_multiplier * p0 + second_multipier * p1 + third_multipier * p2 + fourth_multiplier * p3;
    return acc;
}

QVector3D BezierPath::getDerivative3At(float t) {
    QVector3D p0 = controlPoints[0]->getPosition();
    QVector3D p1 = controlPoints[1]->getPosition();
    QVector3D p2 = controlPoints[2]->getPosition();
    QVector3D p3 = controlPoints[3]->getPosition();
    QVector3D acc = QVector3D();
    float first_multiplier = -6;
    float second_multipier = 18;
    float third_multipier = -18;
    float fourth_multiplier = 6;
    acc = first_multiplier * p0 + second_multipier * p1 + third_multipier * p2 + fourth_multiplier * p3;
    return acc;
}


QVector3D BezierPath::getDerivative4PlusAt(float t) {
    return QVector3D(0,0,0);
}


