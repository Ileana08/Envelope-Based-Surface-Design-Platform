#ifndef CONTROLPOINT_H
#define CONTROLPOINT_H

#include <QVector3D>
#include <QVector>
#include "../vertex.h"

class ControlPoint
{
    QVector3D position;
    float radius;
    int sectors = 20;
    QVector<Vertex> vertexArr;

    constexpr static float PI = 3.14159265;

    public:
        ControlPoint();
        ControlPoint(QVector3D position, float radius, int sectors, QVector<Vertex> vertexArr);

        inline QVector3D getPosition() { return position; }
        inline float getRadius() { return radius; }
        inline void setPosition(QVector3D position) { this->position = position; }
        inline void setRadius(float radius) { this->radius = radius; }
        inline void setSectors(int sectors) { this->sectors = sectors;}
        inline int getSectors(){ return sectors; }

        Vertex getSphereSurfaceAt(float angle, float rotation);
        void computeSphere();
}

#endif //CONTROLPOINT_H
