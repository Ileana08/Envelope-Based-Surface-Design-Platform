#ifndef BEZIERPATH_H
#define BEZIERPATH_H

#include "path.h"
#include "controlpoint.h"
#include <QOpenGLDebugLogger>

class BezierPath : public Path
{
    // cubic: C(t) = (1-t)^3 P0 + 3 (1-t)^2 t P1 + 3 (1-t) t^2 P2+ t^3 P3
    QVector<ControlPoint*> controlPoints;
    
public:
    BezierPath();
    BezierPath(QVector<ControlPoint*> controlPoints);

    QVector3D getPathAt(float t) override;
    void updateVertexArr() override;
    QVector3D getDerivativeAt(float t);
    QVector3D getDerivative2At(float t);
    QVector3D getDerivative3At(float t);
    QVector3D getDerivative4PlusAt(float t);

    inline void setControlPoints(QVector<ControlPoint*> controlPoints) {this->controlPoints = controlPoints; updateVertexArr();}

    inline QVector<ControlPoint*> getControlPoints() {return controlPoints;}
};

#endif // BEZIERPATH_H
