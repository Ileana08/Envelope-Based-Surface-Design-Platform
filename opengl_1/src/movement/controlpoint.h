#ifndef CONTROLPOINT_H
#define CONTROLPOINT_H

#include <QVector3D>
#include <QVector>
#include <QOpenGLDebugLogger>
#include "../vertex.h"

/**
 * @brief The ControlPoint class is a control point parameterized as a sphere.
 */
class ControlPoint
{
    QVector3D position;
    float radius;
    int sectors = 20;
    QVector3D color;

    QVector<Vertex> vertexArr;
    constexpr static float PI = 3.14159265;

    public:
        ControlPoint();
        ControlPoint(QVector3D position, float radius, int sectors);
        ControlPoint(QVector3D position, QVector3D color, float radius, int sectors);

        inline QVector3D getPosition() { return position; }
        inline float getRadius() { return radius; }
        inline void setPosition(QVector3D position) { this->position = position; computeControlPoint(); }
        inline void setRadius(float radius) { this->radius = radius; computeControlPoint(); }
        inline void setSectors(int sectors) { this->sectors = sectors; }
        inline int getSectors(){ return sectors; }
        inline QVector<Vertex> getVertexArr() { return vertexArr; }

        Vertex getSurfaceAt(float angle, float rotation);
        void computeControlPoint();
};

#endif //CONTROLPOINT_H
