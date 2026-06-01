#ifndef ORIENTATIONCONTROLPOINT_H
#define ORIENTATIONCONTROLPOINT_H

#include <QVector3D>
#include <QVector>
#include <QOpenGLDebugLogger>
#include "../vertex.h"
#include "controlpoint.h"

class OrientationControlPoint
{
    QVector3D position;
    float radius;
    int sectors = 20;
    QVector<Vertex> vertexArr;
    ControlPoint* controlPoint;

    constexpr static float PI = 3.14159265;

    public:
        OrientationControlPoint();
        OrientationControlPoint(QVector3D position, float radius, int sectors, ControlPoint* controlPoint);

        inline QVector3D getPosition() { return position; }
        inline float getRadius() { return radius; }
        inline void setPosition(QVector3D position) { this->position = position; computeOrientationPoint(); }
        inline void setRadius(float radius) { this->radius = radius; computeOrientationPoint(); }
        inline void setSectors(int sectors) { this->sectors = sectors; }
        inline int getSectors(){ return sectors; }
        inline QVector<Vertex> getVertexArr() { return vertexArr; }
        inline ControlPoint* getControlPoint() { return controlPoint; }
        inline void setControlPoint(ControlPoint* controlPoint) { this->controlPoint = controlPoint; computeOrientationPoint(); }
        inline float getDistanceToControlPoint() { return (position - controlPoint->getPosition()).length(); }

        Vertex getSurfaceAt(float angle, float rotation);
        void computeOrientationPoint();
        void setPosBasedOnDist(float distance);
};

#endif //ORIENTATIONCONTROLPOINT_H
